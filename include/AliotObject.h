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

    String auth_token;
    String object_id;

} AliotWebSocketConfig;

typedef String AliotEvent_t;
// String constants instead of char* for better string comparison 
struct AliotEvents {
    static inline const AliotEvent_t EVT_CONNECT_OBJECT = "connect_object";
    static inline const AliotEvent_t EVT_ERROR = "error";
    static inline const AliotEvent_t EVT_PING = "ping";
    static inline const AliotEvent_t EVT_PONG = "pong";
    static inline const AliotEvent_t EVT_UPDATE_DOC = "update_doc";
};




class AliotObject {
    public:
        AliotObject();
        ~AliotObject();

        void run();
        void stop();

        // Temporary configuration system
        void setup_config(String auth_token, String object_id, const char* ssid, const char* password);
        void loop_websocket();

        // TODO: Make this more generic
        void send_event(AliotEvent_t event, const char* data);
        void update_doc(const char* data);

        void on_message(uint8_t * payload, size_t length);
        void on_open();
        void on_close();

    private:
        WebSocketsClient m_client;
        AliotWebSocketConfig m_config;
        
        void setup_wifi();
        void setup_websocket();

        WebSocketsClient::WebSocketClientEvent begin_event_listener();

};

#endif /* ALIOT_OBJECT_ H */