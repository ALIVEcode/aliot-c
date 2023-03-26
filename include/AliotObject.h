#ifndef ALIOT_OBJECT_H
#define ALIOT_OBJECT_H

#include <WebSocketsClient.h>
#include <WiFi.h>
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

} AWSConfig_t;

// Aliot WebSocket Event Callback
typedef std::function<void(WStype_t type, uint8_t * payload, size_t length)> AWSClientEvent;

class AliotObject {
    public:
        WebSocketsClient m_AWSClient; // Aliot WebSocket Client

        AWSConfig_t m_AWSConfig; // Aliot WebSocket Config
        bool connected = false;
        
        AliotObject();
        ~AliotObject();

        void run();
        void stop();
        
        // Return a callback function 
        AWSClientEvent register_event();

        // Temporary configuration system
        void setup_config(String auth_token, String object_id, const char* ssid, const char* password);
        void setup_wifi();
        void setup_websocket();

        void send_event(String event);

        void on_message(uint8_t * payload);
        void on_open();
        void on_close();

};

#endif /* ALIOT_OBJECT_ H */