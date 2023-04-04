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

// Update counter for the purposes of this example
int uodateCounter = 0;


void setup() {
    // Choose a timer interval in milliseconds
    aliotObj.timer.setInterval(1000);

    // Configure necessary data for wifi and aliot connection
    aliotObj.setupConfig(authToken, objectId, ssid, password);

    // Start connection process and listen for events
    aliotObj.run();
}

void loop() {
    aliotObj.loop();

    if (aliotObj.timer.wait()) {// Do something after the timer interval has passed
        uodateCounter++;
        aliotObj.updateDoc(createDict<unsigned long>(Pair("/doc/currentTime", millis())));
        aliotObj.updateDoc(createDict<int>(Pair("/doc/updates", uodateCounter)));

        if (!(uodateCounter % 10)) { // After 10 update, increase wait interval by 1 second
            aliotObj.timer.setInterval(aliotObj.timer._interval + 1000);
        }

    }
}