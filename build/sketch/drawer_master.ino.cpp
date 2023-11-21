#include <Arduino.h>
#line 1 "C:\\Users\\c5pas\\Documents\\Arduino\\libraries\\Node\\drawer_master\\drawer_master.ino"
#include "MasterNode.hpp"

MasterNode masterNode;
long int start = 0;
void setup()
{
    Serial.begin(BAUD_RATE);
    masterNode = MasterNode();
}

void loop()
{
    if (millis() - start >= 5000)
    {
        masterNode.hello();
        start = millis()
    }
}

void serialEvent()
{
    masterNode.receivePacket();
}
