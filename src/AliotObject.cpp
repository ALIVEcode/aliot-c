#include "AliotObject.h"


AliotObject::AliotObject() {
    Serial.begin(115200);
    this->m_AWSClient = WebSocketsClient();
}

// Dont call this yet, causes corrupted head error
AliotObject::~AliotObject() {
    this->m_AWSClient.disconnect();
}

void AliotObject::run() {
    this->setup_wifi();
    this->setup_websocket();    
}

void AliotObject::setup_config(String auth_token, String object_id, const char* ssid, const char* password) {
    this->m_AWSConfig.auth_token = auth_token;
    this->m_AWSConfig.object_id = object_id;
    this->m_AWSConfig.ssid = ssid;
    this->m_AWSConfig.password = password;
}

void AliotObject::setup_wifi() {
    WiFi.mode(WIFI_STA);

    WiFi.begin(this->m_AWSConfig.ssid, this->m_AWSConfig.password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

// TODO: Switch to secure connection 
void AliotObject::setup_websocket() {
    this->m_AWSClient.begin(this->m_AWSConfig.host, this->m_AWSConfig.port, this->m_AWSConfig.path);
    this->m_AWSClient.onEvent(this->register_event());
}

// TODO: proper send_event method with json and all
void AliotObject::send_event(String event) {
    String s1 = "{\"event\":\"";
    String s2 = "\",\"data\": {\"id\":\"";
    String s3 = "\",\"token\":\"";
    String s4 = "\"}}";

    // Attempt to avoid buffer overflow error with string concatenation
    String total = "";
    total += s1;
    total += event;
    total += s2;
    total += this->m_AWSConfig.object_id;
    total += s3;
    total += this->m_AWSConfig.auth_token;
    total += s4;

    this->m_AWSClient.sendTXT(total);
}

void AliotObject::on_message(uint8_t * payload) {
    Serial.println("[AWSClient] Message received");
    Serial.println((char*)payload);
}

void AliotObject::on_open() {
    Serial.println("[AWSClient] Connection opened");
    this->send_event("connect_object");
    this->connected = true;
}

void AliotObject::on_close() {
    Serial.println("[AWSClient] Connection closed");
    this->connected = false;
}


AWSClientEvent AliotObject::register_event() {
    // lil hack to use method as callback function
    return [this](WStype_t type, uint8_t * payload, size_t length) {

        switch(type) {

            case WStype_DISCONNECTED:
                this->on_close();
                break;

            case WStype_CONNECTED:
                this->on_open();
                break;
            
            case WStype_TEXT:
                this->on_message(payload);
                break;
            }

            /*
            We won't use other types of events from the webSocketsClient library. Receiving ping pongs 
            through WSType_PING and WSType_PONG will cause a buffer overflow error. Same thing
            for sendPing(). All events have to be received as WStype_TEXT and sent with sendTXT().
            */
    };

}