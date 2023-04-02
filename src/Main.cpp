#include "AliotObject.h"

// Setup WiFi credentials
const char* ssid = "";
const char* password = "";

// Setup Aliot credentials
const char* authToken = "";
const char* objectId = "";

// Create AliotObject instance
AliotObject aliotObj = AliotObject();

void setup() {
  Serial.begin(115200);
  
  aliotObj.setDebugMode(true);

  aliotObj.setupConfig(authToken, objectId, ssid, password);
  aliotObj.run();
  
}

void loop() {
  aliotObj.loop();
}
