#ifndef ALIOT_OBJECT_H
#define ALIOT_OBJECT_H

#include <WebSocketsClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ;;;;;;;;;;; ALIOT CONFIGURATION ;;;;;;;;;;;;
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Aliot WebSocket Configuration 
// Temporary config system to put everything in one place
typedef struct {

    int port = 8881; // TODO: Secure connection
    const char* host = "alivecode.ca";
    const char* path = "/iotgateway/";

    const char* ssid;
    const char* password;

    const char* authToken;
    const char* objectId;

} AliotWebSocketConfig;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ;;;;;;;;;;;;;;; ALIOT EVENTS ;;;;;;;;;;;;;;;
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

typedef const char* AliotEvent_t;
struct AliotEvents {
    static inline AliotEvent_t EVT_CONNECT_SUCCESS = "connect_success";
    static inline AliotEvent_t EVT_CONNECT_OBJECT = "connect_object";
    static inline AliotEvent_t EVT_ERROR = "error";
    static inline AliotEvent_t EVT_PING = "ping";
    static inline AliotEvent_t EVT_PONG = "pong";
    static inline AliotEvent_t EVT_UPDATE_DOC = "update_doc";
};

typedef struct {
    unsigned long _currentTime = 0;
    unsigned long _prevTime = 0;
    unsigned long _interval = 1000;

    void setInterval(unsigned long interval) {
        _interval = interval;
    }

    bool wait() {
        _currentTime = millis();
        if (!(_currentTime % _interval) && _currentTime != _prevTime) {
            _prevTime = _currentTime;
            return true;
        } return false;
    }
} AliotTimer;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ;;;;;;;;;;;;; ALIOT DICTIONARY ;;;;;;;;;;;;;
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

/*
The purpose of the following code is to create a similar data structure to hashmaps (or dictionaries, as seen in the python version of aliot). 
This allows the user to determine key/value pairs in a Json object by directly initializing the pairs in the 
parameters of the createDict function. 
To format json objects correctly, we first deserialize the pairs in createDict(),
serialize it into a string, pass the string in another method, deserialize the string again to change it,
and so on. This allows more flexibility in the value types and number of key/value pairs.
*/

/**
 * NOTE : If it turns out that the above method is too slow or takes too much memory,
 * we can go back to using overloaded methods for updateDoc and sendEvent to handle json related stuff.
 * NOTE 2 : I don't recommend using the std::array versions of createDict. Opt for the regular ones.
*/

// TODO: get rid of strings
typedef String AliotDict_t;

/*
T : Type of the second element of the pair (value in key/)
*/
template<typename T>
struct Pair {
    const char* key;
    T value;

    Pair(const char* key, T value) {
        this->key = key; 
        this->value = value;
    }
};

/*
Pair struct that can handle arrays
T : Type of the second element (value) of the key/value pair
N : Number of elements in the json array
*/
template<typename T, size_t N>
struct Pair<T[N]> {
    const char* key;
    T value[N];

    Pair(const char* key, T value[N]) {
        this->key = key; 
        for (int i = 0; i < N; i++) 
            this->value[i] = value[i];
    }
    // /* Allows to create pair array with brace initializer */
    Pair(const char* key, std::array<T, N> value) {
        this->key = key; 
        for (int i = 0; i < N; i++) 
            this->value[i] = value[i];
    }
};

/*
Create a key/value pair and return a string representation of the JSON object.
Generic type T refers to the value type in the key/value pair.
*/
template<typename T> AliotDict_t createDict(Pair<T> pair) {
    StaticJsonDocument<256> doc; // TODO: Determine doc capacity correctly
    String data; // TODO: get rid of strings

    doc[pair.key] = pair.value;
    serializeJson(doc, data);
    return data;
};

/*
Creates a list of key/array pairs and returns a string representation of the Json Array.
Generic type T refers to the value type in the array. size_t N refers to  
All values in list of arrays must be of same type T. 
All arrays in list must have the same number of elements N.
*/
template<typename T, size_t N> AliotDict_t createDict(Pair<T[N]> arrayPair) {
    StaticJsonDocument<256> doc; // TODO: Determine doc capacity correctly
    String data; // TODO: get rid of strings

    JsonArray arrayHolder = doc.createNestedArray(arrayPair.key);
    arrayHolder.add(arrayPair.key);

    serializeJson(doc, data);
    return data;
}

// ========= WARNING : STD::ARRAY VERSIONS OF createDict ARE NOT RECOMMENDED ==============
// Don't use too much, causes a stack overflow sometimes because of the dynamic initialization.
// I think it's fine for rare use internally, but don't go overboard with calls in loop() yet.
// Just opt for multiple calls to the regular versions of createDict for multiple doc updates
// =========================================================================================

/*
Create a list of key/value pairs and return a string representation of the JSON object.
Generic type T refers to the value type in the key/value pair. All values in list must be of same type.
size_t N refers to the number of key/value pairs in the list.
*/
template<typename T, size_t N> AliotDict_t createDict(std::array<Pair<T>, N> pairList) {
    StaticJsonDocument<256> doc; // TODO: Determine doc capacity correctly
    String data; // TODO: get rid of strings

    // Map each key/value pair in the json document
    for (int i = 0; i < N; i++) 
        doc[pairList[i].key] = pairList[i].value;

    serializeJson(doc, data);
    return data;
};

/*
Creates a list of key/array pairs and returns a string representation of the Json Array.
Generic type T refers to the value type in the array. size_t N refers to  
All values in list of arrays must be of same type T. 
All arrays in list must have the same number of elements N.
*/
template<typename T, size_t N, size_t P> AliotDict_t createDict(std::array<Pair<T[N]>, P> pairArrayList) {
    StaticJsonDocument<256> doc; // TODO: Determine doc capacity correctly
    String data; // TODO : get rid of strings

    // Map each key/array pair in the json document
    for (int i = 0; i < pairArrayList.size(); i++) {
        JsonArray arrayHolder = doc.createNestedArray(pairArrayList[i].key);

        // Add each element in the array
        for (int j = 0; j < N; j++) { 
            arrayHolder.add(pairArrayList[i].value[j]);
        }
    }
    serializeJson(doc, data);
    return data;
};

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ;;;;;;;;;;;; ALIOT OBJECT CLASS ;;;;;;;;;;;;
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

class AliotObject {
    private:
        // Websocket components
        WebSocketsClient _client;
        AliotWebSocketConfig _config;
        

        // State of Aliot Object
        bool _connected;
        bool _validConfig;
        bool _debugMode; 

        int _pingCounter;
        
        // Setup connection
        void setupWiFi();
        void setupWebSocket();

        // Start listening for events through library events, then configure aliot events with on_message
        void beginEventListener();

    public:
        AliotTimer timer;

        AliotObject();
        ~AliotObject();

        /*
        Set to false by default. If true, prints out all received/sent payloads.
        */
        void setDebugMode(bool debugMode);
        bool isConnected();

        void run();
        void stop();


        // Temporary configuration system
        void setupConfig(const char* authToken, const char* objectId, const char* ssid, const char* password);

        /*
        Call in the Arduino loop() function.
        */
        void loop();

        void connectObject();

        void sendEvent(AliotEvent_t event, const char* data);
        void sendEvent(AliotEvent_t event, String data);

        // See updateDoc examples for correct use
        void updateDoc(AliotDict_t aliotDict);

        void updateDoc(const char* key, const char* value);
        void updateDoc(const char* key, int value);
        void updateDoc(const char* key, float value);
        void updateDoc(const char* key, bool value);



        
        void handleEvent(AliotEvent_t event, const char* data);

        void onMessage(uint8_t * payload, size_t length);

        // Only handles "already connected" error for now
        void onError(const char* errorData);

        void onOpen();
        void onClose();
};

#endif /* ALIOT_OBJECT_ H */