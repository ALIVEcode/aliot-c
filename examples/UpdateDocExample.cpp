#include "AliotObject.h"

/**
 * UPDATE DOC EXAMPLE
 * Examples on how to use the updateDoc() method. 
 * The whole process can be written in a single call to updateDoc(), but lets break down the steps for clarity.
 * 
 * /!\ WARNING /!\
 * This file is only an example. Do not send update doc with this much data all at once.
 * Nothing bad happens, but the esp might reset or something. I haven't tested the capacity yet.
 * */

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
    Pair humidityDict = Pair<int>("/doc/humidity", 50);
    Pair temperatureDict = Pair<int>("/doc/temperature", 25);
    Pair luminosityDict = Pair<int>("/doc/luminosity", 100);

    // ==== LIST OF KEY/VALUE PAIRS ====
    std::vector<Pair<int>> pairList = { humidityDict, temperatureDict, luminosityDict };

    // ==== STRING REPRESENTATION OF JSON OBJECT WITH KEY/VALUE PAIRS ====
    AliotDict_t aliotDict = createDict(pairList);

    // ======= UPDATE DOCUMENT =========
    aliotObj.updateDoc(aliot);

    // ====================================
    // PUTTING ALL OF THE ABOVE TOGETHER...
    // ====================================

    aliotObj.updateDoc( // Update document
        createDict<int>( // String representation of JSON object with key/value pairs
            { // List of key/value pairs
                Pair("/doc/humidity", humidity), // Humidity dict
                Pair("/doc/temperature", temperature),  // Temperature dict
                Pair("/doc/luminosity", luminosity) // Luminosity dict
            }
        )
    );

    // ============================
    // SUPPORT FOR OTHER DATA TYPES
    // ============================

    // createDict supports all data types that can be represented with ArduinoJson. 
    // See UpdateDocArrayExample.cpp for examples with json arrays.

    // String values
    aliotObj.updateDoc(
        createDict<const char*>({
            Pair("/doc/example", "Hello World!"),
            Pair("/doc/anotherExample", "Lorem Ipsum")
        }));

    // Boolean values 
    aliotObj.updateDoc(
        createDict<bool>(
            { // YOU NEED BRACES AROUND THE PAIR...
                Pair("/doc/connected", true)
            } // ...EVENT WHEN THERE IS ONLY 1 ELEMENT !
        )
    );

    // Other numbers
    aliotObj.updateDoc(
        createDict<float>({
            Pair("/doc/temperature", 25.5),
            Pair("/doc/humidity", 50.5),
            Pair("/doc/luminosity", 100.5)
        }));

    // etc, etc, etc...
}