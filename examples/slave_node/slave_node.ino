#include "SlaveNode.hpp"

SlaveNode myNode("123456", 13, 3, 4);
void setup() {
  Serial.begin(115200);
  myNode.begin();
  addOperation("Hello", [](uint8_t parameters) {
    Serial.println("hello");
  });
}

void loop() {
  myNode.update();
}
