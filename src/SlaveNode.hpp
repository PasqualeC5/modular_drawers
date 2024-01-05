#ifndef SLAVENODE_HPP
#define SLAVENODE_HPP

#include "Node.hpp"



void addOperation(String operationName, CallbackFunctionByteArray function);

class SlaveNode : public Node
{
private:
    void beginWire();
    void beginStatusLed();

protected:
    unsigned long STATUS_LED_DELAY_MS = 500;
    unsigned int STATUS_LED_PIN = 13;
    unsigned long statusLedTimer;

    bool statusLedState = false;
    virtual void updateStatusLed();

public:
    SlaveNode(const char *sid, unsigned int ledPin, unsigned int ePin, unsigned int fPin);
    bool isConnected();
    void begin();
    void update();
};

// extern void assignAddress(uint8_t assignedAddress);
// extern void requestHandler();
// extern void receiveHandler(int numBytes);

#endif // SLAVENODE_HPP
