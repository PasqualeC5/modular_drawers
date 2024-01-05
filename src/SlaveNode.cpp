#include "SlaveNode.hpp"
#include "SimpleMap.hpp"
// StepperAsync5 *stepper;
// const int stepsPerRevolution = 2038;

SimpleMap<String, CallbackFunctionByteArray> operations;

const char *serialId;
String lastOperation;

uint8_t address = DEFAULT_ADDRESS;
unsigned int forwardPin;
unsigned int enabledPin;

bool isEnabled()
{
    return digitalRead(enabledPin);
}

void assignAddress(uint8_t *parameters, unsigned int nParameters = 1)
{
    Serial.println("---Address received---");
    address = parameters[0];
    Wire.end();
    Wire.begin(address);
    Serial.print("address:\t");
    Serial.println(address);
    digitalWrite(forwardPin, HIGH);
}

void addOperation(String operationName, CallbackFunctionByteArray function)
{
    // operations.insert(std::pair<String, CallbackFunctionByteArray>(operationName, function));
    operations[operationName] = function;
    Serial.print("Added operation: ");
    Serial.println(operationName);
}

void executeOperation(const char *operationName, uint8_t *parameters, unsigned int nParameters)
{

    if (operations.find(operationName))
    {
        lastOperation = String(operationName);
        operations[operationName](parameters, nParameters);
    }
    else
    {
        Serial.print("Operation not found: ");

        lastOperation = String("error");
        Serial.println(operationName);
    }
}

void onReceive(int numBytes)
{
    String operation = "";
    uint8_t parameters[128];
    int nParameters = 0;
    uint8_t receivedByte;
    if (Wire.available())
    {
        receivedByte = Wire.read();
        while (receivedByte != SEPARATION_CHARACTER && Wire.available())
        {
            operation += (char)receivedByte;
            receivedByte = Wire.read();
        }

        if (receivedByte != SEPARATION_CHARACTER)
            return;
        while (Wire.available())
        {
            parameters[nParameters++] = Wire.read();
        }

        Serial.print("Received operation: ");
        Serial.println(operation);
        executeOperation(operation.c_str(), parameters, nParameters);
    }
}

void onRequest()
{
    Wire.write((uint8_t *)serialId, strlen(serialId));
    Wire.write(SEPARATION_CHARACTER);
    Wire.write((uint8_t *)lastOperation.c_str(), lastOperation.length());
    Wire.write(TERMINATION_CHARACTER);
}

SlaveNode::SlaveNode(const char *sid, unsigned int ledPin, unsigned int ePin, unsigned int fPin)
{
    serialId = sid;
    STATUS_LED_PIN = ledPin;
    enabledPin = ePin;
    forwardPin = fPin;
}

// stepper pins are 8 10 9 11

void SlaveNode::beginWire()
{
    addOperation(String("AssignAddress"), assignAddress);
    Wire.begin(address);
    Wire.onReceive(onReceive);
    Wire.onRequest(onRequest);
}

void SlaveNode::beginStatusLed()
{
    pinMode(STATUS_LED_PIN, OUTPUT);
    pinMode(enabledPin, INPUT);
    pinMode(forwardPin, OUTPUT);
    digitalWrite(forwardPin, LOW);
    STATUS_LED_DELAY_MS = 100;

    long int start = millis();
    statusLedTimer = start;
}

void SlaveNode::begin()
{
    beginStatusLed();
    delay(100);
    Serial.println("Waiting for enable signal");
    while (!isEnabled())
    {
        delay(100);
    };
    beginWire();
}

void SlaveNode::update()
{
    updateStatusLed();
}

bool SlaveNode::isConnected()
{
    return address == DEFAULT_ADDRESS;
}

void SlaveNode::updateStatusLed()
{
    if (!isConnected())
        STATUS_LED_DELAY_MS = 500;
    else
        STATUS_LED_DELAY_MS = 100;
    if (millis() - statusLedTimer < STATUS_LED_DELAY_MS)
        return;
    statusLedTimer = millis();
    statusLedState = !statusLedState;
    digitalWrite(STATUS_LED_PIN, statusLedState);
}
