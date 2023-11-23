#include "Node.hpp"
Node myNode;
void setup()
{
    Serial.begin(BAUD_RATE);
    myNode = Node();
    myNode.begin();
}

void loop()
{
    myNode.update();
    
}
