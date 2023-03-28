#include <Arduino.h>
#include "AliotObject.h"


// WiFi credentials

const char* ssid = "";
const char* password = "";


// Aliot credentials
String auth_token = "";
String object_id = "";

// Create AliotObject instance
AliotObject AWSClient = AliotObject();


void setup() {
  Serial.begin(115200);
  pinMode(0, INPUT_PULLUP);
  AWSClient.setup_config(auth_token, object_id, ssid, password);
  AWSClient.run();
}

void loop() {
  AWSClient.m_AWSClient.loop();


}
