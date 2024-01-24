#ifndef ALIOT_OBJECT_H
#define ALIOT_OBJECT_H

#include <WebSocketsClient.h>
#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <ArduinoHttpClient.h>
#include <string>
#include "AliotDict.h"
#include "AliotTimer.h"
#include "ConfigDirectives.h"

typedef const char* AliotEvent_t;
struct AliotEvents {
    static inline AliotEvent_t EVT_CONNECT_SUCCESS = "connect_success";
    static inline AliotEvent_t EVT_CONNECT_OBJECT = "connect_object";
    static inline AliotEvent_t EVT_ERROR = "error";
    static inline AliotEvent_t EVT_PING = "ping";
    static inline AliotEvent_t EVT_PONG = "pong";
    static inline AliotEvent_t EVT_UPDATE_DOC = "update_doc";
    static inline AliotEvent_t EVT_RECEIVE_DOC = "receive_doc";
    static inline AliotEvent_t EVT_SEND_ACTION = "send_action";
    static inline AliotEvent_t EVT_SEND_ACTION_DONE = "send_action_done";
    static inline AliotEvent_t EVT_RECEIVE_ACTION = "receive_action";
    static inline AliotEvent_t EVT_GET_FIELD = "get_field";
};


// Aliot WebSocket Configuration 
// Temporary config system to put everything in one place
typedef struct {
    int port = 8881;
    const char* host = "alivecode.ca";
    const char* path = "/iotgateway/";
    const char* apiPath = "/api";

    const char* ssid;
    const char* password;

    const char* authToken;
    const char* objectId;

    bool modemSleep = true;
} AliotWebSocketConfig;

typedef std::function<void()> AliotEventCallback;

typedef std::function<bool(const char* data)> AliotActionCallback;

typedef struct {
    char* actionId = NULL;
    AliotActionCallback callback;
} AliotAction;

using namespace Dict;

class AliotObject {
    private:
        // Websocket components
        WebSocketsClient _client;
        AliotWebSocketConfig _config;

        AliotEventCallback _onStartCallback;
        AliotEventCallback _onReconnectCallback;
        
        // State of Aliot Object
        bool _connected;
        bool _validConfig;

        AliotAction _actionMap[MAX_ACTION_COUNT];
    
    public:
        WiFiClient wifi;
        AliotTimer timer;

        AliotObject();
        ~AliotObject();
        
        bool isConnected();

        // Connect to WiFi and initialize WebSocket connection
        void run();

        // Test Aliot on local AliveCode server
        void runLocal(const char* IPAdress);

        void stop();

        // Setup connection
        void setupWiFi();
        void setupWebSocket();

        // Start listening for events through library events, then configure aliot events with on_message
        void beginEventListener();

        // Temporary configuration system
        void setupConfig(const char* authToken, const char* objectId, const char* ssid, const char* password);
        void setupConfig(const char* authToken, const char* objectId, const char* ssid, const char* password, const bool modemSleep);

        void setReconnectCallback(AliotEventCallback callback);
        
        /*
        Call in the Arduino loop() function.
        */
        void loop();

        // Return true if connectObject event was sent successfully
        bool connectObject();

        void runBeforeDeepSleep(AliotTimerCallback timerCallback);

        // Return true if event was sent successfully
        bool sendEvent(AliotEvent_t event, const char* data);
        bool sendEvent(AliotEvent_t event, String data);

        // Return true if updateDoc event was sent successfully
        bool updateDoc(AliotDict_t aliotDict);

        String getDoc(String key);

        void handleEvent(AliotEvent_t event, StaticJsonDocument<256> doc);

        void onStart(AliotEventCallback callback);

        void onMessage(uint8_t * payload, size_t length);

        void onActionRecv(const char* actionId, AliotActionCallback callback);
        void onActionRecv(const char* actionId, AliotActionCallback callback, bool log_reception);

        // Only handles "already connected" error for now
        void onError(const char* errorData);

        void onOpen();
        void onClose(); 
};

#endif /* ALIOT_OBJECT_ H */