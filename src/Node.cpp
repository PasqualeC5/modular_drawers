#include "Node.hpp"

uint8_t data[DATA_SIZE];

void assignAddress(uint8_t assignedAddress)
{
    Serial.println("ASSIGNING NEW ADDRESS");
    data[ADDRESS_LOCATION] = assignedAddress;
    Wire.end();
    Wire.begin(data[ADDRESS_LOCATION]);
    Serial.print("NEW ADDRESS:\t");
    Serial.println(data[ADDRESS_LOCATION]);
    data[CONNECTION_STATUS_LOCATION] = CONNECTED;
}

void receiveHandler(int numBytes)
{
    if (Wire.available())
    {
        char operation = Wire.read();
        switch (operation)
        {
        case ASSIGN_ADDRESS:
            assignAddress(Wire.read());
            break;
        default:
            break;
        }
    }
}

void requestHandler()
{
    Wire.write(data, DATA_SIZE);
}

Node::Node()
{
    data[ADDRESS_LOCATION] = DEFAULT_ADDRESS;
    data[CONNECTION_STATUS_LOCATION] = DISCONNECTED;
    data[STATUS_LOCATION] = CLOSED;
    data[LAST_OPERATION_LOCATION] = CONNECTED;

    STATUS_LED_DELAY_MS = 100;

    long int start = millis();
    statusLedTimer = start;
}

void Node::begin()
{
    Wire.begin(data[ADDRESS_LOCATION]);
    Wire.onReceive(receiveHandler);
    Wire.onRequest(requestHandler);
    pinMode(STATUS_LED_PIN, OUTPUT);
}

void Node::update()
{
    updateStatusLed();
}

void Node::updateStatusLed()
{
    if (data[CONNECTION_STATUS_LOCATION] == CONNECTED)
        STATUS_LED_DELAY_MS = 500;
    else
        STATUS_LED_DELAY_MS = 100;
    if (millis() - statusLedTimer < STATUS_LED_DELAY_MS)
        return;
    statusLedTimer = millis();
    statusLedState = !statusLedState;
    digitalWrite(STATUS_LED_PIN, statusLedState);
}
