#include <Arduino.h>
#line 1 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\modular_drawers.ino"

#include <Wire.h>
#include <EEPROM.h>
#define DEFAULT_ADDRESS 69
#define MASTER_ADDRESS 8
#define EE_ISSET_LOCATION 0
#define EE_ADDRESS_LOCATION 1
#define CONFIG 0
#define UPDATE 1
#define DATA 2

void (*resetFunc)(void) = 0;
byte address;

#line 15 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\modular_drawers.ino"
void receiveHandler(int n_bytes);
#line 54 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\modular_drawers.ino"
void requestEvent();
#line 59 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\modular_drawers.ino"
void setup();
#line 82 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\modular_drawers.ino"
void loop();
#line 15 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_master.ino"
void setupFromEEPROM();
#line 31 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_master.ino"
void requestHandler();
#line 31 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_slave_serial.ino"
void discover_address();
#line 69 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_slave_serial.ino"
void handle_communication();
#line 73 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_slave_serial.ino"
void send_packet(Packet packet);
#line 84 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_slave_serial.ino"
void receive_packet();
#line 15 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\modular_drawers.ino"
void receiveHandler(int n_bytes)
{
    byte header = Wire.read();
    byte data;
    switch (header)
    {

    case CONFIG:
        data = Wire.read();
        EEPROM.write(EE_ISSET_LOCATION, byte(1));
        EEPROM.write(EE_ADDRESS_LOCATION, data);

        Serial.print("Received address: ");
        Serial.println(data);
        Serial.flush();
        delay(1000);
        Wire.end();
        Wire.begin(data);
        break;

    case UPDATE:
        data = Wire.read();
        Serial.println("Update request");
        Serial.print("LED status set to: ");
        Serial.println(data);
        digitalWrite(13, data);
        break;

    case DATA:
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
    if (EEPROM.read(EE_ISSET_LOCATION) == byte(1))
    {
        address = EEPROM.read(EE_ADDRESS_LOCATION);
        Serial.print("Address set to: ");
        Serial.println(address);
        Wire.begin(address);
    }
    else
    {
        Serial.println("Address not set in memory");
        Wire.begin(DEFAULT_ADDRESS);
        Serial.println("Waiting for address");
    }

    Wire.onReceive(receiveHandler);
    Wire.onRequest(requestEvent);

    delay(100);
}

void loop()
{
}
#line 1 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_master.ino"
#include <Wire.h>
#include <EEPROM.h>
#define N_INDIRIZZI 255
#define START_ADDRESS 9
#define MASTER_ADDRESS 8
#define DEFAULT_ADDRESS 69
#define SIZE_ADDRESS 0
#define CONFIG 0
#define UPDATE 1
#define DATA 2

byte indirizziCassetti[N_INDIRIZZI];
int n_cassetti = 0;

void setupFromEEPROM()
{
    n_cassetti = EEPROM.read(SIZE_ADDRESS);
    if (n_cassetti == byte(255))
    {
        // first time reading
        n_cassetti = 0;
        EEPROM.write(SIZE_ADDRESS, n_cassetti);
    }

    for (int i = 0; i < n_cassetti; i++)
    {
        indirizziCassetti[i] = EEPROM.read(START_ADDRESS + i);
    }
}

void requestHandler()
{
    byte address = START_ADDRESS + n_cassetti++;
    Wire.beginTransmission(DEFAULT_ADDRESS);
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

    indirizziCassetti[n_cassetti++] = byte(START_ADDRESS + n_cassetti);

    Wire.beginTransmission(DEFAULT_ADDRESS);
    Wire.write(CONFIG);
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
        Wire.write(UPDATE);
        Wire.write(byte(255));
        Wire.endTransmission();

        delay(3000);

        Wire.beginTransmission(indirizziCassetti[i]);
        Wire.write(UPDATE);
        Wire.write(byte(0));
        Wire.endTransmission();
    }
}
#line 1 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_slave.ino"
#include <Wire.h>
#include <EEPROM.h>
#define DEFAULT_ADDRESS 69
#define MASTER_ADDRESS 8
#define EE_ISSET_LOCATION 0
#define EE_ADDRESS_LOCATION 1
#define CONFIG 0
#define UPDATE 1
#define DATA 2

void (*resetFunc)(void) = 0;
byte address;

void receiveHandler(int n_bytes)
{
    byte header = Wire.read();
    byte data;
    switch (header)
    {

    case CONFIG:
        data = Wire.read();
        EEPROM.write(EE_ISSET_LOCATION, byte(1));
        EEPROM.write(EE_ADDRESS_LOCATION, data);

        Serial.print("Received address: ");
        Serial.println(data);
        Serial.flush();
        delay(1000);
        Wire.end();
        Wire.begin(data);
        break;

    case UPDATE:
        data = Wire.read();
        Serial.println("Update request");
        Serial.print("LED status set to: ");
        Serial.println(data);
        digitalWrite(13, data);
        break;

    case DATA:
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
    if (EEPROM.read(EE_ISSET_LOCATION) == byte(1))
    {
        address = EEPROM.read(EE_ADDRESS_LOCATION);
        Serial.print("Address set to: ");
        Serial.println(address);
        Wire.begin(address);
    }
    else
    {
        Serial.println("Address not set in memory");
        Wire.begin(DEFAULT_ADDRESS);
        Serial.println("Waiting for address");
    }

    Wire.onReceive(receiveHandler);
    Wire.onRequest(requestEvent);

    delay(100);
}

void loop()
{
}
#line 1 "C:\\Users\\Pasquale\\Desktop\\arduino_projects\\modular_drawers\\drawer_slave_serial.ino"
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

