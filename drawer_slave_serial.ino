const byte MAX_DATA_SIZE = 10;  // Change the size based on your requirements
#define DISCOVERY_TIMEOUT 10000
#define DISCOVERING 0
#define ASSIGNED 1
const byte BROADCAST_ADDRESS = 255;
const char DISCOVERY_HEADER = 'D';
struct Packet {
    byte header;
    byte senderAddress;
    byte receiverAddress;
    byte dataSize;
    byte data[MAX_DATA_SIZE];
};

unsigned int address_status = DISCOVERING;
byte address = 0;
Packet receivedPacket;

void setup() {
    Serial.begin(9600);
}

void loop() {
    if (address_status == DISCOVERING) {
        discover_address();
    } else {
        // Your main loop logic goes here
    }
}

void discover_address() {
    Packet discoveryPacket;
    discoveryPacket.header = DISCOVERY_HEADER;
    discoveryPacket.senderAddress = address;
    discoveryPacket.receiverAddress = BROADCAST_ADDRESS;
    discoveryPacket.dataSize = 2;  // Example data size
    discoveryPacket.data[0] = 0x42;  // Example data
    discoveryPacket.data[1] = 0x17;  // Example data

    send_packet(discoveryPacket);

    // Wait for an address assignment response
    unsigned long start = millis();
    while (!Serial.available() && millis() - start < DISCOVERY_TIMEOUT) {
        delayMicroseconds(10);
    }

    if (millis() - start >= DISCOVERY_TIMEOUT) {
        return;
    }

    while (address_status == DISCOVERING) {
        receive_packet();

        if (receivedPacket.header == DISCOVERY_HEADER) {
            address = receivedPacket.data[0];
            address_status = ASSIGNED;

            // Send confirmation to the sender
            discoveryPacket.data[0] = address;
            send_packet(discoveryPacket);
        } else {
            // Forward the packet to the next node
            send_packet(receivedPacket);
        }
    }
}

void handle_communication() {
    // Your existing code for handling communication
}

void send_packet(Packet packet) {
    Serial.write(packet.header);
    Serial.write(packet.senderAddress);
    Serial.write(packet.receiverAddress);
    Serial.write(packet.dataSize);

    for (byte i = 0; i < packet.dataSize; ++i) {
        Serial.write(packet.data[i]);
    }
}

void receive_packet() {
    receivedPacket.header = Serial.read();
    receivedPacket.senderAddress = Serial.read();
    receivedPacket.receiverAddress = Serial.read();
    receivedPacket.dataSize = Serial.read();

    for (byte i = 0; i < receivedPacket.dataSize; ++i) {
        receivedPacket.data[i] = Serial.read();
    }
}
