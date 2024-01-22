#include "AliotObject.h"


// Setup wifi credentials
const char* ssid = "";
const char* password = "";

// Setup aliot credentials
const char* authToken = "";
const char* objectId = "";

// Crate AliotObject instance 
AliotObject aliotObj = AliotObject();

// Callback function for "switch" action with the data received from the server
bool callback(const char* data) {
    Serial.println(data);
    return true;
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting...");
    // Configure necessary data for wifi and aliot connection
    aliotObj.setupConfig(authToken, objectId, ssid, password);

    // Create  action listener for the "switch" action
    aliotObj.onActionRecv("switch", callback, true);

    // Start connection process and listen for events
    aliotObj.run();
}

void loop() {
    aliotObj.loop();
}