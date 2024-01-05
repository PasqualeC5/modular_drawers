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
    OnSlaveUpdate _onDisconnect;
    OnSlaveUpdate _onConnect;
    OnSlaveUpdate _onUpdate;

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
    void onConnect(OnSlaveUpdate callback)
    {
        _onConnect = callback;
    }
    void onDisconnect(OnSlaveUpdate callback)
    {
        _onDisconnect = callback;
    }
    void onUpdate(OnSlaveUpdate callback)
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
    void operateOnDevice(uint8_t deviceAddress, const char *deviceOperation, uint8_t *operationParameters, uint8_t nParameters);
    void checkConnectedDevicesStatus();
};

#endif