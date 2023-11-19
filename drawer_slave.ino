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