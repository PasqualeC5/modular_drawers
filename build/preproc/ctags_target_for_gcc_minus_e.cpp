# 1 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\modular_drawers.ino"

# 3 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\modular_drawers.ino" 2
# 4 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\modular_drawers.ino" 2
# 12 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\modular_drawers.ino"
void (*resetFunc)(void) = 0;
byte address;

void receiveHandler(int n_bytes)
{
    byte header = Wire.read();
    byte data;
    switch (header)
    {

    case 0:
        data = Wire.read();
        EEPROM.write(0, byte(1));
        EEPROM.write(1, data);

        Serial.print("Received address: ");
        Serial.println(data);
        Serial.flush();
        delay(1000);
        Wire.end();
        Wire.begin(data);
        break;

    case 1:
        data = Wire.read();
        Serial.println("Update request");
        Serial.print("LED status set to: ");
        Serial.println(data);
        digitalWrite(13, data);
        break;

    case 2:
        break;

    default:
        return;
    }
    while (Wire.available())
    {
    }
}

void requestEvent()
{
    Wire.write('Y');
}

void setup()
{
    Serial.begin(115200);
    if (EEPROM.read(0) == byte(1))
    {
        address = EEPROM.read(1);
        Serial.print("Address set to: ");
        Serial.println(address);
        Wire.begin(address);
    }
    else
    {
        Serial.println("Address not set in memory");
        Wire.begin(69);
        Serial.println("Waiting for address");
    }

    Wire.onReceive(receiveHandler);
    Wire.onRequest(requestEvent);

    delay(100);
}

void loop()
{
}
# 1 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_master.ino"
# 2 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_master.ino" 2
# 3 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_master.ino" 2
# 12 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_master.ino"
byte indirizziCassetti[255];
int n_cassetti = 0;

void setupFromEEPROM()
{
    n_cassetti = EEPROM.read(0);
    if (n_cassetti == byte(255))
    {
        // first time reading
        n_cassetti = 0;
        EEPROM.write(0, n_cassetti);
    }

    for (int i = 0; i < n_cassetti; i++)
    {
        indirizziCassetti[i] = EEPROM.read(9 + i);
    }
}

void requestHandler()
{
    byte address = 9 + n_cassetti++;
    Wire.beginTransmission(69);
    Wire.write("HELLO SLAVE");
    Wire.endTransmission();
    Serial.println("New address request");
    Serial.println(address);
}

void setup()
{
    Serial.begin(115200);
    setupFromEEPROM();
    Wire.begin();
    Wire.onRequest(requestHandler);
    Serial.println("Setup complete");
    delay(2000);

    indirizziCassetti[n_cassetti++] = byte(9 + n_cassetti);

    Wire.beginTransmission(69);
    Wire.write(0);
    Wire.write(indirizziCassetti[0]);
    Wire.endTransmission();

    Serial.print("Address sent: ");
    Serial.println(indirizziCassetti[0]);
}

void loop()
{
    delay(3000);
    for (int i = 0; i < n_cassetti; i++)
    {
        Wire.beginTransmission(indirizziCassetti[i]);
        Wire.write(1);
        Wire.write(byte(255));
        Wire.endTransmission();

        delay(3000);

        Wire.beginTransmission(indirizziCassetti[i]);
        Wire.write(1);
        Wire.write(byte(0));
        Wire.endTransmission();
    }
}
# 1 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_slave.ino"
# 2 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_slave.ino" 2
# 3 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_slave.ino" 2
# 11 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_slave.ino"
void (*resetFunc)(void) = 0;
byte address;

void receiveHandler(int n_bytes)
{
    byte header = Wire.read();
    byte data;
    switch (header)
    {

    case 0:
        data = Wire.read();
        EEPROM.write(0, byte(1));
        EEPROM.write(1, data);

        Serial.print("Received address: ");
        Serial.println(data);
        Serial.flush();
        delay(1000);
        Wire.end();
        Wire.begin(data);
        break;

    case 1:
        data = Wire.read();
        Serial.println("Update request");
        Serial.print("LED status set to: ");
        Serial.println(data);
        digitalWrite(13, data);
        break;

    case 2:
        break;

    default:
        return;
    }
    while (Wire.available())
    {
    }
}

void requestEvent()
{
    Wire.write('Y');
}

void setup()
{
    Serial.begin(115200);
    if (EEPROM.read(0) == byte(1))
    {
        address = EEPROM.read(1);
        Serial.print("Address set to: ");
        Serial.println(address);
        Wire.begin(address);
    }
    else
    {
        Serial.println("Address not set in memory");
        Wire.begin(69);
        Serial.println("Waiting for address");
    }

    Wire.onReceive(receiveHandler);
    Wire.onRequest(requestEvent);

    delay(100);
}

void loop()
{
}
# 1 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_slave_serial.ino"
const byte MAX_DATA_SIZE = 10; // Change the size based on your requirements



const byte BROADCAST_ADDRESS = 255;
const char DISCOVERY_HEADER = 'D';
struct Packet {
    byte header;
    byte senderAddress;
    byte receiverAddress;
    byte dataSize;
    byte data[MAX_DATA_SIZE];
};

unsigned int address_status = 0;
byte address = 0;
Packet receivedPacket;

void setup() {
    Serial.begin(9600);
}

void loop() {
    if (address_status == 0) {
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
    discoveryPacket.dataSize = 2; // Example data size
    discoveryPacket.data[0] = 0x42; // Example data
    discoveryPacket.data[1] = 0x17; // Example data

    send_packet(discoveryPacket);

    // Wait for an address assignment response
    unsigned long start = millis();
    while (!Serial.available() && millis() - start < 10000) {
        delayMicroseconds(10);
    }

    if (millis() - start >= 10000) {
        return;
    }

    while (address_status == 0) {
        receive_packet();

        if (receivedPacket.header == DISCOVERY_HEADER) {
            address = receivedPacket.data[0];
            address_status = 1;

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
