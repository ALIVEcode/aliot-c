#include "AliotObject.h"

/**
 * TIMER EXAMPLE
 * This code is an example on how to use the AliotTimer struct.
 * This is an alternative to using the delay() function.
*/

// Setup wifi credentials
const char* ssid = "";
const char* password = "";

// Setup aliot credentials
const char* authToken = "";
const char* objectId = "";

// Crate AliotObject instance 
AliotObject aliotObj = AliotObject();
AliotTimer aliotTimer = AliotTimer();

// Update counter for the purposes of this example
int updateCounter = 0;

void setup() {
    // Set delayTime to 1 second
    aliotTimer.setDelayTime(1000);

    // Configure necessary data for wifi and aliot connection
    aliotObj.setupConfig(authToken, objectId, ssid, password);

    // Start connection process and listen for events
    aliotObj.run();
}

void loop() {
    aliotObj.loop();

    // Do something only if delayTime has passed
    if (!aliotObj.timer.waitDelayTime()) return;

    updateCounter++;
    aliotObj.updateDoc(createDict<int>(Pair("/doc/updates", updateCounter)));
}