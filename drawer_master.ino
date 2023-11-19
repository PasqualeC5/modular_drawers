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