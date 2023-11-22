#include <MasterNode.hpp>
#include <Node.hpp>

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
        start = millis();
    }
}

void serialEvent()
{
    masterNode.receivePacket();
}