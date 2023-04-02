#include "AliotObject.h"

/**
 * UPDATE DOC EXAMPLE
 * 
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

    //================================
    // DETAILED ANATOMY OF UPDATEDOC()
    //===============================

    // ==== VALUES ====
    int humidity = 50;
    int temperature = 25;
    int luminosity = 100;

    // ==== KEY/VALUE PAIRS ====
    Dict humidityDict = Dict<int>("/doc/humidity", 50);
    Dict temperatureDict = Dict<int>("/doc/temperature", 25);
    Dict luminosityDict = Dict<int>("/doc/luminosity", 100);

    // ==== LIST OF KEY/VALUE PAIRS ====
    std::vector<Dict<int>> dictList = { humidityDict, temperatureDict, luminosityDict };

    // ==== STRING REPRESENTATION OF JSON OBJECT WITH KEY/VALUE PAIRS ====
    AliotDict_t aliotDict = createDict(dictList);

    // ======= UPDATE DOCUMENT =========
    aliotObj.updateDoc(aliotDict);

    // ====================================
    // PUTTING ALL OF THE ABOVE TOGETHER...
    // ====================================

    aliotObj.updateDoc( // Update document
        createDict<int>( // String representation of JSON object with key/value pairs
            { // List of all key/value pairs
                Dict("/doc/humidity", humidity), // Humidity dict
                Dict("/doc/temperature", temperature),  // Temperature dict
                Dict("/doc/luminosity", luminosity) // Luminosity dict
            }
        )
    )

    // ============================
    // SUPPORT FOR OTHER DATA TYPES
    // ============================

    // createDict supports all data types that can be represented . Not arrays though (yet).

    // String values
    aliotObj.updateDoc(
        createDict<const char*>({
            Dict("/doc/example", "Hello World!"),
            Dict("/doc/anotherExample", "Lorem Ipsum")
        }));

    // Boolean values
    aliotObj.updateDoc(createDict<bool>(Dict("/doc/connected", true)));

    // Other numbers
    aliotObj.updateDoc(
        createDict<float>({
            Dict("/doc/temperature", 25.5),
            Dict("/doc/humidity", 50.5),
            Dict("/doc/luminosity", 100.5)
        }));

    // etc, etc, etc...
}