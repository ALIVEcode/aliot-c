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
    // Comment out to disable printing all payload to Serial monitor
    aliotObj.setDebugMode(true);

    // Configure necessary data for wifi and aliot connection
    aliotObj.setupConfig(authToken, objectId, ssid, password);

    // Start connection process and listen for events
    aliotObj.run();
}

void loop() {
    aliotObj.loop();
}
