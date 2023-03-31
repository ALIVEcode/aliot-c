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
typedef String AliotDoc_t;

struct AliotEvents {
    static inline AliotEvent_t EVT_CONNECT_SUCCESS = "connect_success";
    static inline AliotEvent_t EVT_CONNECT_OBJECT = "connect_object";
    static inline AliotEvent_t EVT_ERROR = "error";
    static inline AliotEvent_t EVT_PING = "ping";
    static inline AliotEvent_t EVT_PONG = "pong";
    static inline AliotEvent_t EVT_UPDATE_DOC = "update_doc";
};

// By turning the key-value pair into a string, we can create json documents with different value types
// while having the correct json format for each type, without causing method overload ambiguity.
// Then, we can pass the AliotDoc_t (String) as a parameter in the update doc method, and convert it back to a json object,
// Without the compiler having to know the type of the value parameter.
// All we have to do is pass createDoc<T>(key, value), only needing to specify the type of the second parameter
// when passing it in updateDoc();
template<typename T> AliotDoc_t createDoc(const char* key, T value) {  
    StaticJsonDocument<300> doc;

    // Output target
    AliotDoc_t aliotDocument; 

    // json object can figure out the type of the value itself
    doc["fields"][key] = value;

    // Serialize the json document into a string
    serializeJson(doc, aliotDocument);

    return aliotDocument;
}


class AliotObject {
    public:
        AliotObject();
        ~AliotObject();

        void run();
        void stop();

        // Temporary configuration system
        void setupConfig(const char* authToken, const char* objectId, const char* ssid, const char* password);
        void loop();

        // TODO: Make this more generic
        void sendEvent(AliotEvent_t event, const char* data);
        void sendEvent(AliotEvent_t event, JsonObject& nestedData);

        // Example :
        // - updateDoc(createDoc<int>("/doc/int", 100));
        // - updateDoc(createDoc<double>("/doc/double", 100.0));
        // - updateDoc(createDoc<bool>("/doc/bool", true));
        // - updateDoc(createDoc<const char*>("/doc/string", "Hello World"));
        void updateDoc(AliotDoc_t doc);

        

        void onMessage(uint8_t * payload, size_t length);
        void onError(const char* data);
        void onOpen();
        void onClose();


    private:
        WebSocketsClient mClient;
        AliotWebSocketConfig mConfig;
        
        void setupWiFi();
        void setupWebSocket();

        // Start listening for events through library events, then configure aliot events with on_message
        WebSocketsClient::WebSocketClientEvent beginEventListener();
};

#endif /* ALIOT_OBJECT_ H */