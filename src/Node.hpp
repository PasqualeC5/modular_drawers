#ifndef NODE_HPP
#define NODE_HPP
#include <Arduino.h>
#include <EEPROM.h>

#define BAUD_RATE 9600

class Node
{
protected:
    static const byte BROADCAST_ADDRESS PROGMEM = B11111111;

    static const byte MASTER_ADDRESS PROGMEM = B11111110;

    static const byte DEFAULT_ADDRESS PROGMEM = B11111101;

    static const byte UID_ISSET_ADDRESS PROGMEM = B00000000;

    static const byte UID_ADDRESS PROGMEM = B00000001;

    static const uint16_t UID_LENGTH PROGMEM = 4;

    static const char START_DELIMITER PROGMEM = '@';

    static const char END_DELIMITER PROGMEM = '*';

    static const char DISCOVERY_HEADER PROGMEM = 'D';

    static const char STATUS_HEADER PROGMEM = 'S';

    static const char HELLO_HEADER PROGMEM = 'H';

    static const char OPERATION_HEADER PROGMEM = 'O';

    static const char ERROR_HEADER PROGMEM = 'E';

    static const char REQUEST PROGMEM = 'R';

    static const char ACCEPTED PROGMEM = 'A';

    static const int BAD_PACKET_ERROR = -1;

    static const int BAD_HEADER_ERROR = -2;

    static const byte CLOSED PROGMEM = 0;

    static const byte OPEN PROGMEM = 1;

    static const byte OFFLINE PROGMEM = 0;

    static const byte CONNECTED PROGMEM = 1;

    static const byte DISCOVERING PROGMEM = 2;

    struct Packet
    {
        char startDelimiter = START_DELIMITER; // Start of Packet
        byte sourceAddress;
        byte destinationAddress;
        char operationHeader;
        String data;
        char endDelimter = END_DELIMITER;
        // Constructor for easy packet creation
        Packet(byte src, byte dest, byte header, String d)
            : sourceAddress(src),
              destinationAddress(dest),
              operationHeader(header),
              data(d)
        {
        }
    };

    byte address;
    byte status;
    String uid;
    bool uidSet = false;
    void discoverNetwork();
    int sendPacket(Packet &packet);
    int processPacket(Packet &packet);
    virtual int handleDiscovery(Packet &packet);
    virtual int handleStatus(Packet &packet);
    virtual int handleHello(Packet &packet);
    virtual int handleOperation(Packet &packet);
    String generateUID()
    {
        /* Change to allowable characters */
        const char possible[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
        static char uid[UID_LENGTH + 1];
        for (int p = 0, i = 0; i < UID_LENGTH; i++)
        {
            int r = random(0, strlen(possible));
            uid[p++] = possible[r];
        }
        uid[UID_LENGTH] = '\0';
        return String(uid);
    }
    String read_string_from_eeprom(uint16_t start_address)
    {
        String string;
        for (int i = 0; string[i] != '\0'; i++)
            string[i] = EEPROM[start_address + i];

        return string;
    }

    void write_string_to_eeprom(uint16_t start_address, String string)
    {
        for (int i = 0; i < string.length(); i++)
            EEPROM[start_address + i] = string[i];
        EEPROM[start_address + string.length()] = '\0';
    }

    void printPacket(Packet &p)
    {
        Serial.print("SOURCE:\t");
        Serial.println(p.sourceAddress);
        Serial.print("DESTIN:\t");
        Serial.println(p.destinationAddress);
        Serial.print("HEADER:\t");
        Serial.println(p.operationHeader);
        Serial.print("DATA:\t");
        Serial.println(p.data);
    }

public:
    Node();
    int receivePacket();
};

#endif // NODE_HPP
