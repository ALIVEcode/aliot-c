
#include "AliotObject.h"

// Setup WiFi credentials
const char* ssid = "";
const char* password = "";

// Setup Aliot credentials
const char* authToken = "";
const char* objectId = "";

// Create AliotObject instance
AliotObject aliotObj = AliotObject();

int counter = 0;

void setup() {
    // Choose a timer interval in milliseconds
    aliotObj.timer.setInterval(5000);

    // Configure necessary data for wifi and aliot connection
    aliotObj.setupConfig(authToken, objectId, ssid, password);

    // Start connection process and listen for events
    aliotObj.run();
}

void loop() {
    aliotObj.loop();
}
