#include "MasterNode.hpp"

const int MAX_STRINGS = 10; // Maximum number of substrings
uint8_t onConnectMessage[256];
int splitString(String input, char delimiter, String result[])
{
    int start = 0;
    int length = input.length();
    int count = 0;

    for (int i = 0; i < length; i++)
    {
        if (input[i] == delimiter || i == length - 1)
        {   
            // Extract substring from start to i
            result[count] = input.substring(start, (i == length - 1) ? (i + 1) : i);
            count++;

            // Update start index for the next substring
            start = i + 1;
        }
    }

    return count;
}

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
        operateOnDevice(DEFAULT_ADDRESS, "AssignAddress", &availableAddress, 1);

        Serial.println("---Slave connected---");
        Serial.print("Assigned address:\t");
        Serial.println(availableAddress);
        delay(50);
        Wire.requestFrom(availableAddress, (uint8_t)64);

        index = 0;
        uint8_t byteReceived;
        while (Wire.available() && byteReceived != TERMINATION_CHARACTER)
        {
            byteReceived = Wire.read();
            onConnectMessage[index++] = byteReceived;
        }

        onConnectMessage[index - 1] = '\0';

        Serial.println((char *)onConnectMessage);
        String subStrings[MAX_STRINGS];
        int count = splitString(String((char *)onConnectMessage), SEPARATION_CHARACTER, subStrings);
        if (_onConnect && count > 1)
        {
            _onConnect(availableAddress, subStrings[0], subStrings[1]);
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
                _onDisconnect(addresses[i], String(), "Disconnect");
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
        Wire.requestFrom((uint8_t)addresses[i], (uint8_t)0x7F);
        delay(50);

        index = 0;
        uint8_t byteReceived;
        while (Wire.available() && byteReceived != TERMINATION_CHARACTER)
        {
            byteReceived = Wire.read();
            onConnectMessage[index++] = byteReceived;
        }

        onConnectMessage[index - 1] = '\0';
        String subStrings[MAX_STRINGS];
        int count = splitString(String((char *)onConnectMessage), SEPARATION_CHARACTER, subStrings);

        if (_onUpdate && count > 1)
        {
            _onUpdate(addresses[i], subStrings[0], subStrings[1]);
        }
    }
}

void MasterNode::operateOnDevice(uint8_t deviceAddress, const char *deviceOperation, uint8_t *operationParameters, uint8_t nParameters)
{
    Serial.print("Transmitting to:\t");
    Serial.println(deviceAddress);
    Serial.print("Operation:\t");
    Serial.println(deviceOperation);
    Serial.print("Parameters:\t[");

    for (int i = 0; i < nParameters; i++)
    {
        Serial.print(operationParameters[i]);
        Serial.print(", ");
    }
    Serial.println("]");

    Wire.beginTransmission(deviceAddress);
    Wire.write((const uint8_t *)deviceOperation, strlen(deviceOperation));
    Wire.write(SEPARATION_CHARACTER);
    Wire.write(operationParameters, nParameters);
    Wire.endTransmission();
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
