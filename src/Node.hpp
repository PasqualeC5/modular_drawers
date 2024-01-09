#ifndef NODE_HPP
#define NODE_HPP
#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>
#include <ArduinoJson.h>

#define DEFAULT_ADDRESS 0x7F

#define TERMINATION_CHARACTER 0xFF

const int maxChunkSize = 32;

typedef void (*CallbackFunctionJson)(JsonObject parameters);

inline bool isJsonComplete(String jsonString)
{
    DynamicJsonDocument jsonDoc(256); // Adjust the size based on your JSON structure
    DeserializationError error = deserializeJson(jsonDoc, jsonString);
    if (error)
    {
        jsonDoc.clear();
        return false;
    }
    jsonDoc.clear();
    return true;
}
class Node
{

protected:
    int splitString(String input, char delimiter, String result[])
    {
        int start = 0;
        int length = input.length();
        int count = 0;

        for (int i = 0; i < length; i++)
        {
            if (input[i] == delimiter || i == length - 1)
            {
                // Extract substring from start to i
                result[count] = input.substring(start, (i == length - 1) ? (i + 1) : i);
                count++;

                // Update start index for the next substring
                start = i + 1;
            }
        }

        return count;
    }

public:
    virtual void begin();
    virtual void update();
};

#endif // NODE_HPP
