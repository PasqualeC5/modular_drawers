#include "Node.hpp"

Node::Node()
{
    Serial.flush();

    if (EEPROM[UID_ISSET_ADDRESS] == B00000001 || false)
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
    printPacket(packet);
}

int Node::receivePacket()
{
    if (Serial.read() != (byte)START_DELIMITER)
        return -1;
    byte src = Serial.read();
    byte dest = Serial.read();
    char header = Serial.read();
    String data = Serial.readStringUntil(END_DELIMITER);

    Packet packet(src, dest, header, data);

    if (dest == address || dest == BROADCAST_ADDRESS)
        return processPacket(packet);
    else
    {
        // Pacchetto non è mio quindi lo passo al prossimo nodo nella rete
        return sendPacket(packet);
    }
}

int Node::processPacket(Packet &packet)
{
    Serial.println("PROCESSING PACKET");
    char header = packet.operationHeader;

    printPacket(packet);
    Serial.println("---------------------");

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
    Serial.println("HANDLING DISCOVERY");
    printPacket(packet);
    if (packet.data.substring(0, 4) != uid)
    {
        return sendPacket(packet);
    }
    address = (byte)packet.data[0];

    // EEPROM[UID_ISSET_ADDRESS] = B00000001;
    // EEPROM[UID_ADDRESS] = address;
    // uidSet = true;
    status = CONNECTED;
    Serial.print("Address assigned:\t");
    Serial.print(address);
    Serial.println("CONNECTED");

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
    Serial.println("HANDLING HELLO");
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
