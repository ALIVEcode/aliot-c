#include <Arduino.h>
#include "AliotObject.h"

const char* ssid ="";
const char* password ="";

// Aliot credentials
const char* auth_token = "";
const char* object_id = "";

// Create AliotObject instance
AliotObject AWSClient = AliotObject();


void setup() {
  Serial.begin(115200);
  pinMode(0, INPUT_PULLUP);
   
  AWSClient.setupConfig(auth_token, object_id, ssid, password);
  AWSClient.run();
}

void loop() {
 AWSClient.loopWebSocket();
}
