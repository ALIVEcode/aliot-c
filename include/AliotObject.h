#ifndef ALIOT_OBJECT_H
#define ALIOT_OBJECT_H

#include <WebSocketsClient.h>
#include <Arduino.h>
#include "AliotDict.h"
#include "ConfigDirectives.h"

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ;;;;;;;;;;;;;;; ALIOT EVENTS ;;;;;;;;;;;;;;;
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

typedef const char* AliotEvent_t;

// Choose between using macros or static variables for event constants
// Allows to test performance/memory usage difference between the two methods
#ifndef ALIOT_EVENTS_MACROS
namespace AliotEvents {
    static inline AliotEvent_t EVT_CONNECT_SUCCESS = "connect_success";
    static inline AliotEvent_t EVT_CONNECT_OBJECT = "connect_object";
    static inline AliotEvent_t EVT_ERROR = "error";
    static inline AliotEvent_t EVT_PING = "ping";
    static inline AliotEvent_t EVT_PONG = "pong";
    static inline AliotEvent_t EVT_UPDATE_DOC = "update_doc";
};
// Macros and static variables will have the same name regardless of chosen directive
using namespace AliotEvents;
#endif /* ALIOT_EVENTS_MACROS */


// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ;;;;;;;;; WEBSOCKET CONFIGURATION ;;;;;;;;;;
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// Aliot WebSocket Configuration 
// Temporary config system to put everything in one place
typedef struct {

    int port = 8881; // TODO: Secure connection
    const char* host = "alivecode.ca";
    const char* path = "/iotgateway/";

    const char* ssid;
    const char* password;
    bool modemSleep = true;

    const char* authToken;
    const char* objectId;

} AliotWebSocketConfig;

// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// ;;;;;;;;;;;;;;; ALIOT TIMER ;;;;;;;;;;;;;;;;
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

typedef struct {
    uint32_t _currentTime = 0;
    uint32_t _prevTime = 0;
    uint32_t _interval = 1000;

    void setInterval(uint32_t interval) {
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
// ;;;;;;;;;;;; ALIOT OBJECT CLASS ;;;;;;;;;;;;
// %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

using namespace Dict;

class AliotObject {
    private:
        // Websocket components
        WebSocketsClient _client;
        AliotWebSocketConfig _config;
        
        // State of Aliot Object
        bool _connected;
        bool _validConfig;

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
        void setupConfig(const char* authToken, const char* objectId, const char* ssid, const char* password, const bool modemSleep);

        /*
        Call in the Arduino loop() function.
        */
        void loop();

        void connectObject();

        void sendEvent(AliotEvent_t event, const char* data);
        void sendEvent(AliotEvent_t event, String data);

        // See updateDoc examples for correct use
        void updateDoc(AliotDict_t aliotDict);

        void handleEvent(AliotEvent_t event, const char* data);

        void onMessage(uint8_t * payload, size_t length);

        // Only handles "already connected" error for now
        void onError(const char* errorData);

        void onOpen();
        void onClose();
};

#endif /* ALIOT_OBJECT_ H */