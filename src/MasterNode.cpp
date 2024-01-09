#include "MasterNode.hpp"
#include "SimpleMap.hpp"

void MasterNode::setupForwardPin()
{
    pinMode(forwardPin, OUTPUT);
    digitalWrite(forwardPin, LOW);
}

MasterNode::MasterNode(unsigned int fPin)
{
    forwardPin = fPin;
}

void MasterNode::begin()
{
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        addresses[i] = DEFAULT_ADDRESS;
    }
    unsigned long start = millis();
    scanDevicesTimer = start;
    scanNewDevicesTimer = start;
    checkDevicesStatusTimer = start;
    setupForwardPin();
    Wire.begin();
    digitalWrite(forwardPin, HIGH);
}

void MasterNode::update()
{
    scanForNewDevices();
    scanConnectedDevices();
    checkConnectedDevicesStatus();
    updateStatusLed();
}

void MasterNode::scanForNewDevices()
{

    if (millis() - scanNewDevicesTimer < SCAN_NEW_DEVICES_PERIOD_MS)
        return;
    int index = 0;
    scanNewDevicesTimer = millis();
    int error;

    Wire.beginTransmission(DEFAULT_ADDRESS);
    delay(100);
    error = Wire.endTransmission();
    if (error == 0)
    {
        // New device found
        Serial.println("---New slave found---");
        delay(100);
        uint8_t availableAddress = getAvailableAddress();
        if (availableAddress == DEFAULT_ADDRESS)
        {
            Serial.println("MAX NUMBER OF DEVICES CONNECTED");
            return;
        }
        // operateOnDevice(DEFAULT_ADDRESS, "AssignAddress", &availableAddress, 1);
        const int capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(3);
        DynamicJsonDocument jsonDoc(capacity);
        jsonDoc["operation"] = String("AssignAddress");
        JsonObject parameters = jsonDoc.createNestedObject("parameters");
        parameters["address"] = availableAddress;
        String jsonString;
        serializeJson(jsonDoc, jsonString);
        jsonDoc.clear();

        sendJson(DEFAULT_ADDRESS, jsonString);

        Serial.println("---Slave connected---");
        Serial.print("Assigned address:\t");
        Serial.println(availableAddress);
        delay(50);

        if (_onConnect)
        {
            String receivedMessage = "";
            while (!isJsonComplete(receivedMessage))
            {
                Wire.requestFrom(availableAddress, (uint8_t)32);
                uint8_t receivedByte = 0;
                while (Wire.available() && receivedByte != TERMINATION_CHARACTER)
                {
                    receivedByte = Wire.read();
                    receivedMessage += (char)receivedByte;
                }
            }
            receivedMessage[receivedMessage.length()-1] = '\0';
            jsonDoc = DynamicJsonDocument(256);
            Serial.println(receivedMessage);
            deserializeJson(jsonDoc, receivedMessage.c_str());
            _onConnect(jsonDoc.as<JsonObject>());
            jsonDoc.clear();
        }

        addresses[availableAddress - 1] = availableAddress;
    }
}

void MasterNode::scanConnectedDevices()
{
    int index = 0;
    if (millis() - scanDevicesTimer < SCAN_CONNECTED_DEVICES_PERIOD_MS)
        return;
    scanDevicesTimer = millis();
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        if (addresses[i] == DEFAULT_ADDRESS)
            continue;
        Wire.beginTransmission(addresses[i]);
        if (Wire.endTransmission() != 0)
        {
            if (_onDisconnect)
            {
                DynamicJsonDocument jsonDoc(256);
                jsonDoc["address"] = addresses[i];
                jsonDoc["last_operation"] = String("Disconnect");
                _onDisconnect(jsonDoc.as<JsonObject>());
                jsonDoc.clear();
            }

            addresses[i] = DEFAULT_ADDRESS;
        }
    }
}

void MasterNode::checkConnectedDevicesStatus()
{
    int index = 0;

    if (millis() - checkDevicesStatusTimer < PRINT_CONNECTED_DEVICES_STATUS_PERIOD_MS)
        return;
    checkDevicesStatusTimer = millis();
    for (int i = 0; i < MAX_DEVICES; i++)
    {

        if (addresses[i] == DEFAULT_ADDRESS)
            continue;

        if (_onUpdate)
        {
            DynamicJsonDocument jsonDoc(256);
            String receivedMessage = "";
            while (!isJsonComplete(receivedMessage))
            {
                Wire.requestFrom(addresses[i], (uint8_t)32);
                uint8_t receivedByte = 0;
                while (Wire.available() && receivedByte != TERMINATION_CHARACTER)
                {
                    receivedByte = Wire.read();
                    receivedMessage += (char)receivedByte;
                }
            }
            receivedMessage[receivedMessage.length()-1] = '\0';
            jsonDoc = DynamicJsonDocument(256);
            deserializeJson(jsonDoc, receivedMessage.c_str());

            _onUpdate(jsonDoc.as<JsonObject>());

            jsonDoc.clear();
        }
    }
}

void MasterNode::sendJson(uint8_t deviceAddress, String jsonString)
{
    Serial.print("Json string length: ");
    Serial.println(jsonString.length());
    Serial.println(jsonString);

    int strLength = jsonString.length();
    for (int i = 0; i < strLength; i += maxChunkSize)
    {
        sendChunk(jsonString.substring(i, i + maxChunkSize), deviceAddress);
        delay(100); // Delay to ensure proper transmission, adjust as needed
    }
}

uint8_t MasterNode::getAvailableAddress()
{
    uint8_t i = 0;
    while (i < MAX_DEVICES && addresses[i] != DEFAULT_ADDRESS)
        i++;
    if (i == MAX_DEVICES)
        return DEFAULT_ADDRESS;
    return i + 1;
}

void MasterNode::updateStatusLed()
{
}
