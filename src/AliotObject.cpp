#include "AliotObject.h"

AliotObject::AliotObject() {
    this->_client = WebSocketsClient();

    this->_connected = false;
    this->_validConfig = false;

    this->_pingCounter = 0;

    #ifdef ALIOT_DEBUG
        Serial.begin(115200);
    #endif
}

// Dont call this yet, causes corrupted head error
AliotObject::~AliotObject() {
    this->_client.disconnect();
}

bool AliotObject::isConnected() {
    return this->_connected;
}

void AliotObject::run() {
    if (this->_validConfig) {
        this->setupWiFi();
        this->setupWebSocket();  
    } else {
        Serial.println("[ERROR] Invalid configuration. Call setupConfig() before run()");
    }
}

void AliotObject::setupConfig(const char* authToken, const char* objectId, const char* ssid, const char* password) {
    this->_config.authToken = authToken;
    this->_config.objectId = objectId;
    this->_config.ssid = ssid;
    this->_config.password = password;

    this->_validConfig = true;
}

void AliotObject::setupConfig(const char* authToken, const char* objectId, const char* ssid, const char* password, bool modemSleep) {
    setupConfig(authToken, objectId, ssid, password);
    this->_config.modemSleep = modemSleep;
}

void AliotObject::setupWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(this->_config.ssid, this->_config.password);
    WiFi.setSleep(this->_config.modemSleep);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
}

// TODO: Switch to secure connection 
void AliotObject::setupWebSocket() {
    this->_client.begin(this->_config.host, this->_config.port, this->_config.path);
    this->beginEventListener();

    #ifdef ALIOT_DEBUG
        Serial.println("[DEBUG] Debug mode enabled");
    #endif
}

void AliotObject::loop() {
    this->_client.loop();
}

void AliotObject::onOpen() {
    // onOpen and connectObject are different methods, just in case we'll add sth else to onOpen
    this->connectObject();
}

void AliotObject::onClose() {

}

void AliotObject::onMessage(uint8_t * payload, size_t length) { 
    #ifdef ALIOT_DEBUG
        Serial.println((char*) payload);
    #endif

    StaticJsonDocument<256> doc;
    deserializeJson(doc, (char*) payload);
    this->handleEvent(doc["event"], doc["data"]);
}

void AliotObject::onError(const char* data) {
    #ifdef ALIOT_DEBUG
        Serial.println(data);
    #endif

    bool equalString = false;
    bool startComparison = false;   
    int startIndex = 0;
    int equalCharCounter = 0; 

    for (int i = 0; i < strlen(data); i++) {
        if (data[i] == '"') {
            // True for start quote, false for final quote
            startComparison = !startComparison;
            startIndex = i + 1;
        }

        if (startComparison) { 
            // Check if error message contains object Id
            equalString = (data[i] == this->_config.objectId[i - startIndex]);

            // If true, increment counter ([x+1]*1), else reset to 0 ([x+0]*0)
            equalCharCounter = (equalCharCounter + equalString) * equalString;
        }
    }

    if (equalCharCounter == strlen(this->_config.objectId)) { 
        // Wait before reconnecting
        delay(3000);
        this->connectObject();
    }
}

void AliotObject::sendEvent(AliotEvent_t event, const char* data) {
    String output;
    // Need 2 json docs for this operation
    // One to transfer string data to so that we can format it correctly
    StaticJsonDocument<500> dataHolderDoc;
    deserializeJson(dataHolderDoc, data);

    // One to hold the complete formatted payload that will be sent
    StaticJsonDocument<500> payloadDoc;

    payloadDoc["event"] = event;

    // === Conditional formatting ===
    // Update doc event requires data to be in "fields"
    if (!strcmp(event, EVT_UPDATE_DOC)) {
        payloadDoc["data"]["fields"] = dataHolderDoc.as<JsonObject>();
    } else { 
        payloadDoc["data"] = dataHolderDoc.as<JsonObject>();
    }   

    // Re-convert the formatted payload to string again
    serializeJson(payloadDoc, output);

    // send payload
    //this->_client.sendTXT(buff);
    this->_client.sendTXT(output.c_str());

    #ifdef ALIOT_DEBUG
        Serial.println(output);
    #endif
}

void AliotObject::sendEvent(AliotEvent_t event, String data) {
    // Exists only in case we forget to call c_str() 
    this->sendEvent(event, data.c_str());
}

void AliotObject::updateDoc(AliotDict_t aliotDict) {
    if (this->_connected) 
        this->sendEvent(EVT_UPDATE_DOC, aliotDict);
}

void AliotObject::connectObject() { 
    this->sendEvent(EVT_CONNECT_OBJECT,
        createDict<const char*, 2>({
            Pair("token", this->_config.authToken),
            Pair("id", this->_config.objectId)
    }));
}

void AliotObject::handleEvent(AliotEvent_t event, const char* data) {

    // Reminder : strcmp returns 0 if strings are equal

    if (!strcmp(event, EVT_PING)) {
        this->sendEvent(EVT_PONG, "");
        this->_pingCounter++;
        this->updateDoc(createDict<int>(Pair("/doc/ping", this->_pingCounter)));
    }

    else if (!strcmp(event, EVT_ERROR)) {
        this->onError(data); // Handles only "already connected" error
    }

    else if (!strcmp(event, EVT_CONNECT_SUCCESS)) {
        this->_connected = true;
    }
}

void AliotObject::beginEventListener() {
    // Set lambda function as WebSocketClientEvent callback
    this->_client.onEvent(
        [this](WStype_t type, uint8_t * payload, size_t length) {
            switch(type) {
                case WStype_DISCONNECTED: 
                    this->onClose(); break;

                case WStype_CONNECTED: 
                    this->onOpen(); break;

                case WStype_TEXT: 
                    this->onMessage(payload, length); break;
                
                /*
                We won't use other types of events from the webSocketsClient library. Receiving ping pongs 
                through WSType_PING and WSType_PONG will cause a buffer overflow error. Same thing
                for sendPing(). All events have to be received as WStype_TEXT and sent with sendTXT().
                */
               
                case WStype_ERROR: break; // Not sure if we need this one
                case WStype_BIN: break;
                case WStype_FRAGMENT_TEXT_START: break;
                case WStype_FRAGMENT_BIN_START: break;
                case WStype_FRAGMENT: break;
                case WStype_FRAGMENT_FIN: break;
                case WStype_PING: break;
                case WStype_PONG: break;
            }
        }
    );
}
