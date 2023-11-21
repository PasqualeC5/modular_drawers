#include "Node.hpp"
Node myNode;
void setup()
{
    Serial.begin(BAUD_RATE);
    myNode = Node();
}

void loop()
{
    
}

void serialEvent(){
    myNode.receivePacket();
}