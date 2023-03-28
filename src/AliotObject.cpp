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
    this->m_AWSClient.onEvent(this->begin_event_listener());
}

void AliotObject::on_open() {
    Serial.println("[AWSClient] Connected to AWS");
    StaticJsonDocument<500> doc;

    doc["event"] = "connect_object";

    doc["data"]["token"] = this->m_AWSConfig.auth_token;
    doc["data"]["id"] = this->m_AWSConfig.object_id;

    String output;
    serializeJson(doc, output);

    Serial.println(output.c_str());

    this->m_AWSClient.sendTXT(output.c_str());
}

void AliotObject::on_close() {
    Serial.println("[AWSClient] Disconnected from AWS");
}

void AliotObject::on_message(uint8_t * payload, size_t length) {
    char* message = (char*)payload;

    Serial.println("[AWSClient] Received message from AWS");
    Serial.println(message);

    StaticJsonDocument<200> doc;
    deserializeJson(doc, message);

    String event = doc["event"];
    String data = doc["data"];

    // Testing ping pong
    if (event.equals("ping")) {
        this->send_event("pong", data.c_str());
    }
}

void AliotObject::send_event(const char* event, const char* data) {
    StaticJsonDocument<200> doc;

    doc["event"] = event;
    doc["data"] = data;

    String output;
    serializeJson(doc, output);

    Serial.println(output.c_str());

    this->m_AWSClient.sendTXT(output.c_str());
}

WebSocketsClient::WebSocketClientEvent AliotObject::begin_event_listener() {
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
                this->on_message(payload, length);
                break;
            
            case WStype_ERROR:
                break;

            /*
            We won't use other types of events from the webSocketsClient library. Receiving ping pongs 
            through WSType_PING and WSType_PONG will cause a buffer overflow error. Same thing
            for sendPing(). All events have to be received as WStype_TEXT and sent with sendTXT().
            */
        }
    };
}