#include "AliotObject.h"

/**
 * DEEP SLEEP EXAMPLE
 * This code is an example on how to use the deepSleep function of the AliotTimer struct.
 * In this example, the ESP wakes up, sends an update, goes into deep sleep, and resets again after a certain amount of time.
*/

// Setup wifi credentials
const char* ssid = "";
const char* password = "";

// Setup aliot credentials
const char* authToken = "";
const char* objectId = "";

// Create AliotObject instance 
AliotObject aliotObj = AliotObject();

// Define the timer callback function
// The timer callback must return a boolean value at the instant it should go to sleep 
bool deepSleepCallback() {
    Serial.println("This will be executed only once");
    
    // updateDoc() returns true if the update was successful
    // This gives time for the update to be registered before going into deep sleep
    return aliotObj.updateDoc(createDict<const char*>(Pair("/doc/test", "Hello World!")));
}

void setup() {
    // Configure necessary data for wifi and aliot connection
    aliotObj.setupConfig(authToken, objectId, ssid, password);

    // Start connection process and listen for events
    aliotObj.run();

    // Set the duration of the deep sleep
    aliotObj.timer.setSleepTime(10000);

    // Execute the callback once before going to sleep
    aliotObj.runBeforeDeepSleep(deepSleepCallback);
}

void loop() {
    // By calling runBeforeDeepSleep in setup(), the program never reaches loop()
}