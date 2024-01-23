#include "AliotObject.h"

/**
 * MINIMAL PROJECT EXAMPLE
 * This code is the minimal code required for a working Aliot project.
 * It connects the device to the WiFi network, to the WebSocket server, and to the IoT object on alivecode.ca.
*/

// Setup wifi credentials
const char* ssid = "";
const char* password = "";

// Setup aliot credentials
const char* authToken = "";
const char* objectId = "";

// Crate AliotObject instance 
AliotObject aliotObj = AliotObject();

void setup() {
    // Configure necessary data for wifi and aliot connection
    aliotObj.setupConfig(authToken, objectId, ssid, password);

    // Start connection process and listen for events
    aliotObj.run();
}

void loop() {
    aliotObj.loop();
}