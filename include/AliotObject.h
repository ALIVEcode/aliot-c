#ifndef ALIOT_OBJECT_H
#define ALIOT_OBJECT_H

#include <WebSocketsClient.h>
#include <Arduino.h>
#include <ArduinoJson.h>

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


typedef const char* AliotEvent_t;
struct AliotEvents {
    static inline AliotEvent_t EVT_CONNECT_SUCCESS = "connect_success";
    static inline AliotEvent_t EVT_CONNECT_OBJECT = "connect_object";
    static inline AliotEvent_t EVT_ERROR = "error";
    static inline AliotEvent_t EVT_PING = "ping";
    static inline AliotEvent_t EVT_PONG = "pong";
    static inline AliotEvent_t EVT_UPDATE_DOC = "update_doc";
};


// ==================
// Aliot Dictionary
// ==================

/*
The purpose of the following code is to create key/value pairs in a Json object
by directly initializing the pairs in the parameters of the createDict function.
To format json objects correctly, we first deserialize the pairs in createDict(),
serialize it into a string, pass the string in another method, deserialize the string again to change it,
and so on. This allows more flexibility in the value types.
*/

typedef String AliotDict_t;

template<typename T>
struct Dict {
    const char* key;
    T value;

    Dict(const char* key, T value) {
        this->key = key; 
        this->value = value;
    }
};

/*
Create a list of key/value pairs and return a string representation of the JSON object.
Generic type T refers to the value type in the key/value pair. All values in list must be of same type.
*/
template<typename T> AliotDict_t createDict(std::vector<Dict<T>> dictList) {
    StaticJsonDocument<500> doc;
    String data; // TODO: get rid of strings

    for (int i = 0; i < dictList.size(); i++) 
        doc[dictList[i].key] = dictList[i].value;
    
    serializeJson(doc, data);
    return data;
};

template<typename T> AliotDict_t createDict(Dict<T> dict) {
    return createDict(std::vector<Dict<T>> {dict});
};

// ==================
// ALIOT OBJECT CLASS
// ==================

class AliotObject {
    public:
        

        AliotObject();
        ~AliotObject();

        /*
        Set to false by default. If true, prints out all received/sent payloads.
        */
        void setDebugMode(bool debugMode);

        void run();
        void stop();


        // Temporary configuration system
        void setupConfig(const char* authToken, const char* objectId, const char* ssid, const char* password);
        void loop();

        void connectObject();

        void sendEvent(AliotEvent_t event, const char* data);
        void sendEvent(AliotEvent_t event, String data);

        void updateDoc(AliotDict_t aliotDict);

        
        void handleEvent(AliotEvent_t event, const char* data);

        void onMessage(uint8_t * payload, size_t length);

        // Only handles "already connected" error for now
        void onError(const char* errorData);

        void onOpen();
        void onClose();

    private:
        WebSocketsClient _client;
        AliotWebSocketConfig _config;

        bool _connected;
        bool _validConfig;
        bool _debugMode; 
        
        void setupWiFi();
        void setupWebSocket();

        // Start listening for events through library events, then configure aliot events with on_message
        WebSocketsClient::WebSocketClientEvent beginEventListener();
};

#endif /* ALIOT_OBJECT_ H */