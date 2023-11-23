#include <MasterNode.hpp>
#include <Node.hpp>

MasterNode masterNode;
long int start = 0;

void setup()
{
    Serial.begin(BAUD_RATE);
    masterNode = MasterNode();
    masterNode.begin();
}

void loop()
{
    if (millis() - start >= 5000)
    {
        masterNode.printConnectedDevicesStatus();
    }
    masterNode.update();
}
