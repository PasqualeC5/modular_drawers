#line 1 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\Node.cpp"
#include "Node.hpp"
#include "Arduino.h"
#include "eeprom.h"

Node::Node()
{
    address = DEFAULT_ADDRESS;
    if (EEPROM[UID_ISSET_ADDRESS] == B00000001)
    {
        uid = read_string_from_eeprom(UID_ADDRESS);
        uidSet = true;
    }
    else
    {
        uid = String("");
        uidSet = false;
    }
    discoverNetwork();
}

void Node::discoverNetwork()
{
    status = DISCOVERING;
    String data(address);
    data = String(data + uidSet);
    Packet packet(address, BROADCAST_ADDRESS, DISCOVERY_HEADER, data);
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
    if (packet.sourceAddress != address)
    {
        packet.data[0]++;
        return sendPacket(packet);
    }
    address = packet.data[0];
    status = CONNECTING;

    packet.sourceAddress = address;
    packet.destinationAddress = MASTER_ADDRESS;
    packet.operationHeader = CONNECTION_HEADER;
    packet.data[0] = address;
    packet.data = String(packet.data + REQUEST + uid);

    return sendPacket(packet);
}

int Node::handleConnection(Packet &packet)
{
    char message = packet.data[0];
    if (message == ACCEPTED)
    {
        uid = String(packet.data.substring(1));
        EEPROM[UID_ISSET_ADDRESS] = B00000001;
        write_string_to_eeprom(UID_ADDRESS, uid);
        status = CONNECTED;
        return status;
    }

    discoverNetwork();
    return -1;
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
        return 0;
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
