#ifndef NODE_HPP
#define NODE_HPP
#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#include <stdint.h>

#define BROADCAST_ADDRESS 0x00
#define MASTER_ADDRESS 0x01
#define DEFAULT_ADDRESS 0x7F

#define ADDRESS_LOCATION 0
#define CONNECTION_STATUS_LOCATION 1
#define STATUS_LOCATION 2
#define LAST_OPERATION_LOCATION 3

#define STATUS_CHECK 'S'
#define ASSIGN_ADDRESS 'A'

#define WAITING 'W'
#define CONNECTED 'C'
#define DISCONNECTED 'D'
#define OPEN 'O'
#define CLOSED 'C'

#define BAUD_RATE 9600
const uint8_t DATA_SIZE = 4;

class Node
{
protected:
    unsigned long STATUS_LED_DELAY_MS = 500;
    unsigned int STATUS_LED_PIN = 13;
    unsigned long statusLedTimer;
    
    bool statusLedState = false;
    void updateStatusLed();
    

public:
    Node();
    void begin();
    virtual void update();
};

extern uint8_t data[DATA_SIZE];
extern unsigned long connectionStatusTimer;
extern void assignAddress(uint8_t assignedAddress);
void updateConnectionStatus();
extern void requestHandler();
extern void receiveHandler(int numBytes);

#endif // NODE_HPP
