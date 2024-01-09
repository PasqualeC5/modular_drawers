#include "SlaveNode.hpp"
#include "ArduinoJson.h"

SlaveNode myNode("123456", 13, 3, 4);
void setup() {
  Serial.begin(115200);
  myNode.begin();
  addOperation("Hello", [](JsonObject parameters) {
    const char* message = parameters["message"];
    Serial.println(message);
  });
}

void loop() {
  myNode.update();
}
