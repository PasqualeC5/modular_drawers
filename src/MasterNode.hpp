#ifndef MASTER_NODE_HPP
#define MASTER_NODE_HPP

#include "Node.hpp"


#define MAX_DEVICES 100
#define SCAN_NEW_DEVICES_PERIOD_MS 2000
#define SCAN_CONNECTED_DEVICES_PERIOD_MS 2000

class MasterNode : public Node
{

protected:
    unsigned long scanNewDevicesTimer = 0;
    unsigned long scanDevicesTimer = 0;
    uint8_t address;

    uint8_t addresses[MAX_DEVICES];
    int connectedDevices;

    void scanForNewDevices();
    void scanConnectedDevices();
    uint8_t getAvailableAddress();

public:
    MasterNode();
    void update();
    void operateOnDevice(uint8_t deviceAddress, uint8_t deviceOperation);
    void printConnectedDevicesStatus();
};

#endif