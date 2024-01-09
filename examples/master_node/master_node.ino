#include <MasterNode.hpp>


MasterNode masterNode(2);
uint8_t connectedAddresses[MAX_DEVICES];
uint8_t* params;
int n = 0;
unsigned long timer = millis();

void onUpdate(JsonObject data) {
  Serial.println("Update");
  uint8_t address = data["address"];
  const char* serial_id = data["serial_id"];
  const char* last_operation = data["last_operation"];
  Serial.println(address);
  Serial.println(serial_id);
  Serial.println(last_operation);
}


void onConnect(JsonObject data) {
  Serial.println("Connect");
  uint8_t address = data["address"];
  const char* serial_id = data["serial_id"];
  const char* last_operation = data["last_operation"];
  Serial.println(address);
  Serial.println(serial_id);
  Serial.println(last_operation);
}

void setup() {
  Serial.begin(115200);
  masterNode.onConnect(onConnect);
  masterNode.onUpdate(onUpdate);
  masterNode.begin();
}

void loop() {
  n = masterNode.getAddresses(connectedAddresses);
  if (millis() - timer >= 3000) {
    for (int i = 0; i < n; i++) {
      const int capacity = JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(2);
      Serial.print("Capacity: ");
      Serial.println(capacity);
      DynamicJsonDocument jsonDoc(capacity);
      jsonDoc["operation"] = "Hello";
      JsonObject parameters = jsonDoc.createNestedObject("parameters");
      parameters["message"] = "World";
      parameters["data"] = 0;
      String jsonString;
      serializeJson(jsonDoc, jsonString);
      jsonDoc.clear();
      masterNode.sendJson(connectedAddresses[i], jsonString);
    }
    timer = millis();
  }
  masterNode.update();
}
