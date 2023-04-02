#include "AliotObject.h"

/**
 * UPDATE DOC ARRAY EXAMPLE
 * Examples on how to use the updateDoc() method with array.
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

    //============================================
    // DETAILED ANATOMY OF UPDATEDOC() WITH ARRAYS
    //============================================

    // ==== DETERMINE THE NUMBER OF ELEMENTS IN THE ARRAYS ====
    // Array size must be the same in for all arrays in the list
    const size_t ARRAY_SIZE = 3;

    // ==== ARRAYS ====
    int humidity[ARRAY_SIZE] = { 50, 60, 70 };
    int temperature[ARRAY_SIZE] = { 25, 26, 27 };
    int luminosity[ARRAY_SIZE] = { 100, 200, 300 };

    // ==== KEY/VALUE PAIRS ====
    Pair humidityPairArray = Pair<int[ARRAY_SIZE]>("/doc/humidity", humidity);
    Pair temperaturePairArray = Pair<int[ARRAY_SIZE]>("/doc/temperature", temperature);
    Pair luminosityPairArray = Pair<int[ARRAY_SIZE]>("/doc/luminosity", luminosity);

    // ==== LIST OF KEY/VALUE PAIRS ====
    std::vector<Pair<int[ARRAY_SIZE]>> pairList = { humidityPairArray, temperaturePairArray, luminosityPairArray };

    // ==== STRING REPRESENTATION OF JSON OBJECT WITH KEY/VALUE PAIRS ====
    AliotDict_t aliotDict = createDict<int, ARRAY_SIZE>(pairList);

    // ======= UPDATE DOCUMENT =========
    aliotObj.updateDoc(aliotDict);

    // ====================================
    // PUTTING ALL OF THE ABOVE TOGETHER...
    // ====================================

    // The following code is equivalent to the above code.
    // Unfortunately, unlike updateDoc() with regular values, you have to specify 
    // the types in both the createDict template arguments, and the Pair template arguments.

    aliotObj.updateDoc(
        createDict<int, ARRAY_SIZE>({
            Pair<int[ARRAY_SIZE]>("/doc/humidity", { 50, 60, 70 }),
            Pair<int[ARRAY_SIZE]>("/doc/temperature", { 25, 26, 27 }),
            Pair<int[ARRAY_SIZE]>("/doc/luminosity", { 100, 200, 300 })
        })
    );

    // ============================
    // SUPPORT FOR OTHER DATA TYPES
    // ============================

    // Float arrays
    aliotObj.updateDoc(
        createDict<float, 5>({
            Pair<float[5]>("/doc/float1", { 50.0, 60.0, 70.0, 80.0, 90.0 }),
            Pair<float[5]>("/doc/float2", { 25.0, 26.0, 27.0, 28.0, 29.0 }),
            Pair<float[5]>("/doc/float3", { 100.0, 200.0, 300.0, 400.0, 500.0 })
        })
    );

    // const char arrays
    aliotObj.updateDoc(
        createDict<const char*, 26>(
            { // YOU NEED BRACES AROUND THE PAIR ARRAY...
                Pair<const char*[26]>("/doc/alphabet", 
                    {"a", "b", "c", "d", "e", "f", "g", "h", "i", 
                    "j","k", "l", "m", "n", "o", "p", "q", "r", 
                    "s", "t","u", "v", "w", "x", "y", "z"})
            } // ... EVENT WHEN THERE IS ONLY 1 ELEMENT !
        )
    );
}
