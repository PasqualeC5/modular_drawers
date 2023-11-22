#ifndef MASTER_NODE_HPP
#define MASTER_NODE_HPP

#include "Node.hpp"

class MasterNode : public Node
{

protected:
    static const uint16_t MAX_DEVICES = 254;
    // char connectedNodes[MAX_DEVICES][UID_LENGTH]
    String addresses = "";
    byte totalDevices = 0;
    uint16_t connectedDevices = 0;
    bool areAddressesStored = false;
    int handleDiscovery(Packet &packet) override;
    int handleHello(Packet &packet);
    int handleStatus(Packet &packet);
    String generateUID();

public:
    void hello();
    void checkStatus();
    MasterNode();
};

#endif