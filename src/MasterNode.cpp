#include "MasterNode.hpp"

MasterNode::MasterNode() : Node()
{
    address = MASTER_ADDRESS;
    if (EEPROM[UID_ISSET_ADDRESS] == B00000001 || false)
    {
        addresses = read_string_from_eeprom(UID_ADDRESS);
        areAddressesStored = true;
    }
    else
    {
        areAddressesStored = false;
        addresses = "";
    }
}

int MasterNode::handleDiscovery(Packet &packet)
{
    // New device added to network and given a unique address
    byte givenAddress = ++totalDevices;

    // addresses =
    //     addresses.substring(0, (givenAddress - 1) * 2) +
    //     packet.data + String((char)CONNECTED) +
    //     addresses.substring(givenAddress * 2, addresses.length());
    // write_string_to_eeprom(UID_ADDRESS + (givenAddress - 1) * 2, packet.data + String((char)CONNECTED));

    packet.destinationAddress = packet.sourceAddress;
    packet.sourceAddress = address;
    packet.operationHeader = DISCOVERY_HEADER;
    packet.data = packet.data + String((char)givenAddress);

    sendPacket(packet);
    Serial.println("NEW DISCOVERY REQUEST");
    Serial.print("TOTAL DEVICES:\t");
    Serial.println(totalDevices);
    printPacket(packet);
}

int MasterNode::handleHello(Packet &packet)
{
    if (packet.sourceAddress == address)
    {
        Serial.println("--------------------------------");
        printPacket(packet);
        Serial.println("--------------------------------");
        return 0;
    }
    int len = packet.data.length();
    packet.data = String(packet.data + "Hello from master: " + address + "\n");
    return packet.data.length() - len;
}

int MasterNode::handleStatus(Packet &packet)
{
    int status = packet.data[0];
    byte src = packet.sourceAddress;
    addresses =
        addresses.substring(0, (src - 1) * 2) +
        packet.data + String((char)CONNECTED) +
        addresses.substring(src * 2, addresses.length());
    // write_string_to_eeprom(UID_ADDRESS + (src - 1) * 2, packet.data + String((char)status));
}

void MasterNode::hello()
{
    if (totalDevices > 0)
    {
        Packet p(MASTER_ADDRESS, BROADCAST_ADDRESS, HELLO_HEADER, "");
        sendPacket(p);
    }
}
