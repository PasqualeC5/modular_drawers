#include <MasterNode.hpp>

MasterNode masterNode(2);
uint8_t connectedAddresses[MAX_DEVICES];
uint8_t *params;
int n = 0;
unsigned long timer = millis();

void onUpdate(uint8_t *parameters, unsigned int nParameters) {
  for (int i = 0; i < nParameters; i++) {
    Serial.print((char)parameters[i]);
  }
  Serial.println();
}

void onConnect(uint8_t *parameters, unsigned int nParameters) {
  Serial.println("Slave connected callback");
}

void setup() {
  Serial.begin(115200);
  masterNode.onConnect(onConnect);
  masterNode.onUpdate(onUpdate);
  masterNode.begin();
}

void loop() {
  n = masterNode.getAddresses(connectedAddresses);
  if (millis() - timer >= 10000) {
    for (int i = 0; i < n; i++) {
      uint8_t numberOfRevs = 1;
      uint8_t speed = 15;
      masterNode.operateOnDevice(connectedAddresses[i], "SetSpeed", &speed, 1);
      masterNode.operateOnDevice(connectedAddresses[i], "OpenDrawer", &numberOfRevs, 1);
      delay(1000);
      masterNode.operateOnDevice(connectedAddresses[i], "StopDrawer", NULL, 0);
      masterNode.operateOnDevice(connectedAddresses[i], "CloseDrawer", &numberOfRevs, 1);

    }
    timer = millis();
  }
  masterNode.update();
}
