#ifndef MASTER_NODE_HPP
#define MASTER_NODE_HPP

#include "Node.hpp"

#define MAX_DEVICES 100
#define SCAN_NEW_DEVICES_PERIOD_MS 1000
#define SCAN_CONNECTED_DEVICES_PERIOD_MS 2000
#define PRINT_CONNECTED_DEVICES_STATUS_PERIOD_MS 2000
#define PRINT_STATUS false


class MasterNode : public Node
{

private:
    CallbackFunctionJson _onDisconnect;
    CallbackFunctionJson _onConnect;
    CallbackFunctionJson _onUpdate;

    void sendChunk(String chunk, uint8_t deviceAddress)
    {
        Wire.beginTransmission(deviceAddress);
        Wire.write((uint8_t *)chunk.c_str(), chunk.length());
        Wire.endTransmission();
    }

protected:
    unsigned int forwardPin;
    unsigned long scanNewDevicesTimer = 0;
    unsigned long scanDevicesTimer = 0;
    unsigned long checkDevicesStatusTimer = 0;

    uint8_t addresses[MAX_DEVICES];

    void scanForNewDevices();
    void scanConnectedDevices();
    uint8_t getAvailableAddress();
    void updateStatusLed();
    void setupForwardPin();

public:
    void onConnect(CallbackFunctionJson callback)
    {
        _onConnect = callback;
    }
    void onDisconnect(CallbackFunctionJson callback)
    {
        _onDisconnect = callback;
    }
    void onUpdate(CallbackFunctionJson callback)
    {
        _onUpdate = callback;
    }
    int getAddresses(uint8_t *connectedAddresses)
    {
        int n = 0;
        for (int i = 0; i < MAX_DEVICES; i++)
        {
            if (addresses[i] != DEFAULT_ADDRESS)
                connectedAddresses[n++] = addresses[i];
        }
        return n;
    }
    MasterNode(unsigned int forwardPin);
    void update();
    void begin();
    void sendJson(uint8_t deviceAddress, String jsonString);
    void checkConnectedDevicesStatus();
};

#endif