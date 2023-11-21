#line 1 "C:\\Users\\c5pas\\Documents\\Arduino\\libraries\\Node\\drawer_master\\Node.cpp"
#include "Node.hpp"
#include "Arduino.h"
#include "eeprom.h"

Node::Node()
{

    if (EEPROM[UID_ISSET_ADDRESS] == B00000001)
    {
        address = EEPROM[UID_ADDRESS];
        uidSet = true;
    }
    else
    {
        address = DEFAULT_ADDRESS;
        uidSet = false;
    }
    discoverNetwork();
}

void Node::discoverNetwork()
{
    status = DISCOVERING;
    uid = generateUID();
    Packet packet(address, BROADCAST_ADDRESS, DISCOVERY_HEADER, uid);
    sendPacket(packet);
}

int Node::receivePacket()
{

    if (Serial.read() != START_DELIMITER)
    {
        // Serial.flush();
        return BAD_PACKET_ERROR;
    }

    byte src = Serial.read();
    byte dest = Serial.read();
    byte header = Serial.read();
    String data = Serial.readStringUntil(END_DELIMITER);

    Packet packet(src, dest, header, data);

    if (dest == address || dest == BROADCAST_ADDRESS)
        return processPacket(packet);
    else
        // Pacchetto non è mio quindi lo passo al prossimo nodo nella rete
        return sendPacket(packet);
}

int Node::processPacket(Packet &packet)
{
    char header = packet.operationHeader;
    // In base all'header scegli che operazione eseguire
    switch (header)
    {
    case DISCOVERY_HEADER:
        return handleDiscovery(packet);
        break;
    case STATUS_HEADER:
        return handleStatus(packet);
        break;
    case HELLO_HEADER:
        return handleHello(packet);
        break;
    case OPERATION_HEADER:
        return handleOperation(packet);
        break;
    default:
        return BAD_HEADER_ERROR;
    }
    // Pacchetto è in broadcast quindi lo invio al prossimo nodo finchè non ritorna al mittente
    if (packet.sourceAddress != address)
        return sendPacket(packet);
}

int Node::handleDiscovery(Packet &packet)
{
    if (packet.data != uid)
    {
        return sendPacket(packet);
    }
    address = packet.data[0];

    EEPROM[UID_ISSET_ADDRESS] = B00000001;
    EEPROM[UID_ADDRESS] = address;
    uidSet = true;
    status = CONNECTED;
    return status;
}

int Node::handleStatus(Packet &packet)
{
    if (packet.destinationAddress != BROADCAST_ADDRESS)
    {
    }
}

int Node::handleHello(Packet &packet)
{
    if (packet.sourceAddress == address)
    {
        Serial.println("--------------------------------");
        Serial.println(packet.data);
        Serial.println("--------------------------------");
        return 0;
    }
    int len = packet.data.length();
    packet.data = String(packet.data + "Hello from node: " + address + "\n");
    return packet.data.length() - len;
}

int Node::handleOperation(Packet &packet)
{
}

int Node::sendPacket(Packet &packet)
{
    size_t nBytes = 0;

    nBytes += Serial.write(packet.startDelimiter);
    nBytes += Serial.write(packet.sourceAddress);
    nBytes += Serial.write(packet.destinationAddress);
    nBytes += Serial.write(packet.operationHeader);
    for (uint32_t i = 0; i < packet.data.length(); i++)
        nBytes += Serial.write(packet.data[i]);
    nBytes += Serial.write(packet.endDelimter);

    return nBytes;
}
