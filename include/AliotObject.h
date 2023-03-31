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


        // Update document with overloaded params
        // TODO: Implement templates instead. Same exact code for each type,
        // don't need method overload
        void updateDoc(const char* key, const char* value);
        void updateDoc(const char* key, int value);
        void updateDoc(const char* key, double value);
        void updateDoc(const char* key, bool value);

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