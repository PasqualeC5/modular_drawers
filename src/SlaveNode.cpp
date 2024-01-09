#include "SlaveNode.hpp"
#include "SimpleMap.hpp"
// StepperAsync5 *stepper;
// const int stepsPerRevolution = 2038;

SimpleMap<String, CallbackFunctionJson> operations;
String receivedData = "";
String requestedData = "";
int requestedDataPosition = 0;


const char *serialId;
String lastOperation;

uint8_t address = DEFAULT_ADDRESS;
unsigned int forwardPin;
unsigned int enabledPin;

bool isEnabled()
{
    return digitalRead(enabledPin);
}

void assignAddress(JsonObject parameters)
{
    Serial.println("---Address received---");
    address = parameters["address"];
    Wire.end();
    Wire.begin(address);
    Serial.print("address:\t");
    Serial.println(address);
    digitalWrite(forwardPin, HIGH);
}

void addOperation(String operationName, CallbackFunctionJson function)
{
    // operations.insert(std::pair<String, CallbackFunctionByteArray>(operationName, function));
    operations[operationName] = function;
    Serial.print("Added operation: ");
    Serial.println(operationName);
}

void executeOperation(const char *operationName, JsonObject parameters)
{

    if (operations.find(operationName))
    {
        lastOperation = String(operationName);
        operations[operationName](parameters);
    }
    else
    {
        Serial.print("Operation not found: ");

        lastOperation = String("error");
        Serial.println(operationName);
    }
}

void processJson(String jsonString)
{
    Serial.println("Processing json");
    Serial.println(jsonString);
    // Parse the JSON string into a JSON object
    DynamicJsonDocument jsonDoc(256); // Adjust the size based on your JSON structure
    DeserializationError error = deserializeJson(jsonDoc, jsonString);

    // Check for parsing errors
    if (error)
    {
        Serial.print("Error parsing JSON: ");
        Serial.println(error.c_str());
        return;
    }
    JsonObject jsonObject = jsonDoc.as<JsonObject>();

    const char *operation = jsonObject["operation"];
    JsonObject parameters = jsonObject["parameters"].as<JsonObject>();
    executeOperation(operation, parameters);
    jsonDoc.clear();
}

void onReceive(int numBytes)
{

    // Read the incoming JSON chunk
    while (Wire.available())
    {
        char c = Wire.read();
        receivedData += c;
    }

    // Check if the end of the JSON string is reached
    if (isJsonComplete(receivedData))
    {
        
        // Complete JSON string received, process it
        processJson(receivedData);
        receivedData = ""; // Reset for the next transmission
    }
}

void onRequest()
{
    if (requestedDataPosition == 0)
    {
        requestedData = "";
        DynamicJsonDocument jsonDoc(256);
        jsonDoc["address"] = address;
        jsonDoc["serial_id"] = String(serialId);
        jsonDoc["last_operation"] = lastOperation;

        serializeJson(jsonDoc, requestedData);
    }
    String chunk = requestedData.substring(requestedDataPosition, requestedDataPosition + maxChunkSize);

    requestedDataPosition += chunk.length();

    Wire.write((uint8_t *)chunk.c_str(), chunk.length());
    if (requestedDataPosition >= requestedData.length())
    {
        requestedDataPosition = 0;
    }
    delay(100);
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
