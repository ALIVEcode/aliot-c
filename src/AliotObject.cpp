#include "AliotObject.h"


AliotObject::AliotObject() {
    Serial.begin(115200);
    this->m_client = WebSocketsClient();
}



// Dont call this yet, causes corrupted head error
AliotObject::~AliotObject() {
    this->m_client.disconnect();
}

void AliotObject::run() {
    this->setup_wifi();
    this->setup_websocket();    
}

void AliotObject::setup_config(String auth_token, String object_id, const char* ssid, const char* password) {
    this->m_config.auth_token = auth_token;
    this->m_config.object_id = object_id;
    this->m_config.ssid = ssid;
    this->m_config.password = password;
}

void AliotObject::setup_wifi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(this->m_config.ssid, this->m_config.password);

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
    this->m_client.begin(this->m_config.host, this->m_config.port, this->m_config.path);
    this->m_client.onEvent(this->begin_event_listener());
}

void AliotObject::loop_websocket() {
    this->m_client.loop();
}

void AliotObject::on_open() {

    Serial.println("[AWSClient] Connected to AWS");
    StaticJsonDocument<500> doc;

    doc["event"] = AliotEvents::EVT_CONNECT_OBJECT;
    JsonObject data = doc.createNestedObject("data");


    data["token"] = this->m_config.auth_token;
    data["id"] = this->m_config.object_id;

    String output;
    serializeJson(doc, output);

    Serial.println(output.c_str());

    this->m_client.sendTXT(output.c_str());
    this->update_doc("\"/doc/test/\":\"test\"");
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
    if (event.equals(AliotEvents::EVT_PING)) {
        this->send_event(AliotEvents::EVT_PONG, "null");
    }



}

void AliotObject::update_doc(const char* data) {
    this->send_event(AliotEvents::EVT_UPDATE_DOC, data);
}

void AliotObject::send_event(AliotEvent_t event, const char* data) {
    StaticJsonDocument<200> doc;

    String output;

    doc["event"] = event;
    doc["data"] = data;
    serializeJson(doc, output);

    Serial.println(output.c_str());

    this->m_client.sendTXT(output.c_str());
}

// Start listening for events through library events, then configure aliot events with on_message
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