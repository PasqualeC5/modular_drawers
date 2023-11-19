#ifndef MASTER_NODE_HPP
#define MASTER_NODE_HPP

#include "Node.hpp"
#include "Node.cpp"

class MasterNode : public Node
{


protected:
    static const uint16_t ADDRESSES_LOCATION = B00000000;
    static const uint16_t MAX_DEVICES = 30;
    String connectedNodes = "0000000000000000000000000000000000000000000000000000000000000000";
    int handleConnection(Packet &packet) override;
    String generateUID();

public:
};

#endif