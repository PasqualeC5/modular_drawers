#ifndef NODE_HPP
#define NODE_HPP
#include <Arduino.h>
#include <Wire.h>
#include <stdint.h>


#define DEFAULT_ADDRESS 0x7F

#define SEPARATION_CHARACTER '@'
#define TERMINATION_CHARACTER 0xFF

typedef void (*CallbackFunctionByteArray)(uint8_t *, unsigned int nParameters);
typedef void (*OnSlaveUpdate)(uint8_t address, String serialId, String lastOperation );

class Node
{

public:
    virtual void begin();
    virtual void update();
};

#endif // NODE_HPP
