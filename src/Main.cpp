#include "AliotObject.h"

// Setup wifi credentials
std::vector<std::string> ssid = {};
std::vector<std::string> password = {};

// Setup aliot credentials
const char* authToken = "";
const char* objectId = "";

// Create AliotObject instance 
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



