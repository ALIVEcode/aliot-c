#include <Arduino.h>
#include "AliotObject.h"


// WiFi credentials

const char* ssid = "";
const char* password = "";


// Aliot credentials
String objectId = "";
String authToken = "";

// Create AliotObject instance
AliotObject AWSClient = AliotObject();


void setup() {
  Serial.begin(115200);
  pinMode(0, INPUT_PULLUP);
  AWSClient.setupConfig(authToken, objectId, ssid, password);
  AWSClient.run();
}

void loop() {
  AWSClient.loop();


}
