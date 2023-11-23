#include "MasterNode.hpp"
#include "Node.hpp"

MasterNode::MasterNode() : Node()
{
    address = MASTER_ADDRESS;
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        addresses[i] = DEFAULT_ADDRESS;
    }
    connectedDevices = 0;
    unsigned long start = millis();
    scanDevicesTimer = start;
    scanNewDevicesTimer = start;
}

void MasterNode::update()
{
    scanForNewDevices();
    scanConnectedDevices();
}

void MasterNode::scanForNewDevices()
{
    if (millis() - scanNewDevicesTimer < SCAN_NEW_DEVICES_PERIOD_MS)
        return;
    scanNewDevicesTimer = millis();
    Serial.println("SCANNING FOR NEW DEVICES");
    int error;
    do
    {
        Wire.beginTransmission(DEFAULT_ADDRESS);
        delay(5);
        error = Wire.endTransmission();
        if (error == 0)
        {
            // New device found
            Serial.println("NEW DEVICE FOUND");
            Wire.beginTransmission(DEFAULT_ADDRESS);
            Wire.write(ASSIGN_ADDRESS);
            uint8_t assignedAddress = getAvailableAddress();
            Wire.write(assignedAddress);
            if (Wire.endTransmission() == 0)
                addresses[assignedAddress - 1] = assignedAddress;
            connectedDevices++;
            delay(50);
        }
        delay(10);
    } while (error == 0);
}

void MasterNode::scanConnectedDevices()
{
    if (millis() - scanDevicesTimer < SCAN_CONNECTED_DEVICES_PERIOD_MS)
        return;
    scanDevicesTimer = millis();
    Serial.println("SCANNING CONNECTED DEVICES");
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        if (addresses[i] == DEFAULT_ADDRESS)
            continue;
        Wire.beginTransmission(addresses[i]);
        if (Wire.endTransmission() != 0)
        {
            Serial.print("DEVICE NOT RESPONDING AT ADDRESS:\t");
            Serial.println(addresses[i]);
            // device not responding
            // free address space
            addresses[i] = DEFAULT_ADDRESS;
            connectedDevices--;
        }
        else
        {
            Serial.print("DEVICE FOUND AT ADDRESS:\t");
            Serial.println(addresses[i]);
        }
    }
}

void MasterNode::printConnectedDevicesStatus()
{
    for (int i = 0; i < MAX_DEVICES; i++)
    {
        if (addresses[i] == DEFAULT_ADDRESS)
            continue;
        Serial.println("REQUESTING DATA");
        Wire.requestFrom(addresses[i], DATA_SIZE);
        uint8_t dataIndex = 0;
        while (Wire.available() && dataIndex < DATA_SIZE)
            data[dataIndex++] = Wire.read();

        if (dataIndex == DATA_SIZE)
        {
            Serial.println("----DEVICE----");
            Serial.print("Address:\t");
            Serial.println(data[0]);
            Serial.print("Connection:\t");
            Serial.println((char)data[1]);
            Serial.print("Status:\t");
            Serial.println((char)data[2]);
            Serial.print("LastOp:\t");
            Serial.println((char)data[3]);
            Serial.println("--------------");
        }
    }
}

void MasterNode::operateOnDevice(uint8_t deviceAddress, uint8_t deviceOperation)
{
    Wire.beginTransmission(deviceAddress);
    Wire.write(deviceOperation);
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
