#include "AliotWebSocket.h"

AliotWebSocket::AliotWebSocket() {
    this->m_AWSClient = WebSocketsClient();
}

AliotWebSocket::~AliotWebSocket() {
    this->m_AWSClient.disconnect();
}

void AliotWebSocket::run() {
    this->m_AWSClient.begin("alivecode.ca", 8881, "/iotgateway/");   
}

void AliotWebSocket::event() {
    std::function<void(WStype_t, uint8_t*, size_t)> cb = 
    [this](WStype_t type, uint8_t * payload, size_t length){
        switch(type) {
            case WStype_DISCONNECTED:
                Serial.printf("[WSc] Disconnected!\n");
                break;
            case WStype_CONNECTED:
            {
                Serial.printf("[WSc] Connected to url: %s\n", payload);
                this->m_AWSClient.sendTXT(
				R"({"event": "connect_object", "data": { "id": "9a38c8ff-45f6-49dd-b38b-0cd1d0fdf981", "token":"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJuYW1lIjoiRGVmYXVsdCBBdXRoVG9rZW4iLCJvYmplY3RJZCI6IjlhMzhjOGZmLTQ1ZjYtNDlkZC1iMzhiLTBjZDFkMGZkZjk4MSIsImlhdCI6MTY3OTUwODIwMywiZXhwIjo0ODMzMTA4MjAzfQ.wQXHcyV-aHhuQdOLntp4kk1TPORsasa5Gp4DAoXsvQk"}})");

                break;
            }

        }
    };

    this->m_AWSClient.onEvent(cb);
}










