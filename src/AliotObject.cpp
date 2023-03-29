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
    this->setupWiFi();
    this->setupWebSocket();    
}

void AliotObject::setupConfig(String authToken, String objectId, const char* ssid, const char* password) {
    this->m_config.authToken = authToken;
    this->m_config.objectId = objectId;
    this->m_config.ssid = ssid;
    this->m_config.password = password;
}

void AliotObject::setupWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(this->m_config.ssid, this->m_config.password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    const char* x = "connect_object";

}

// TODO: Switch to secure connection 
void AliotObject::setupWebSocket() {
    this->m_client.begin(this->m_config.host, this->m_config.port, this->m_config.path);
    this->m_client.onEvent(this->beginEventListener());
}

void AliotObject::loopWebSocket() {
    this->m_client.loop();
}


void AliotObject::onOpen() {
    Serial.println("[AWSClient] Connected to AWS");

    StaticJsonDocument<500> doc;

    doc["event"] = AliotEvents::EVT_CONNECT_OBJECT;
    JsonObject data = doc.createNestedObject("data");

    data["token"] = this->m_config.authToken;
    data["id"] = this->m_config.objectId;

    String output;
    serializeJson(doc, output);

    Serial.println(output.c_str());

    this->m_client.sendTXT(output.c_str());
}

void AliotObject::onClose() {
    Serial.println("[AliotWS] Disconnected from AliotWS");
}

void AliotObject::onMessage(uint8_t * payload, size_t length) {
    char* message = (char*)payload;

    Serial.println("[AliotWS] Received message from AliotWS");
    Serial.println(message);

    StaticJsonDocument<200> doc;
    deserializeJson(doc, message);

    String event = doc["event"];
    String data = doc["data"];
    // Testing ping pong
    if (event.equals(AliotEvents::EVT_PING)) {
        this->sendEvent(AliotEvents::EVT_PONG, "null");
    }

    
    else if (event.equals(AliotEvents::EVT_CONNECT_SUCCESS)) {
 
        this->updateDoc({"/doc/test", "Hello World"});
        
        
    }
}

void AliotObject::sendEvent(const AliotEvent_t& event, const char* data) {
    StaticJsonDocument<200> doc;
    String output;
    doc["event"] = event;
    doc["data"] = data;
    serializeJson(doc, output);
    Serial.println(output.c_str());
    this->m_client.sendTXT(output.c_str());
}

void AliotObject::sendEvent(const AliotEvent_t& event, JsonObject& nestedData) {
    StaticJsonDocument<200> doc;
    String output;
    doc["event"] = event;

    doc.createNestedObject("data");
    doc["data"] = nestedData;

    serializeJson(doc, output);
    Serial.println(output.c_str());

    this->m_client.sendTXT(output.c_str());
}

void AliotObject::updateDoc(std::pair<const char*, const char*> dict) {
    StaticJsonDocument<300> doc;
    JsonObject temp = doc.createNestedObject("temp");
    temp.createNestedObject("fields");
    temp["fields"][dict.first] = dict.second;
    this->sendEvent(AliotEvents::EVT_UPDATE_DOC, temp);

}

WebSocketsClient::WebSocketClientEvent AliotObject::beginEventListener() {
    // lil hack to use method as callback function
    return [this](WStype_t type, uint8_t * payload, size_t length) {

        switch(type) {

            case WStype_DISCONNECTED:
                this->onClose();
                break;

            case WStype_CONNECTED:
                this->onOpen();
                break;
            
            case WStype_TEXT:
                this->onMessage(payload, length);
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