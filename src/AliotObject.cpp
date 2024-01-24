#include "AliotObject.h"

AliotObject::AliotObject() {
    this->_client = WebSocketsClient();

    this->_connected = false;
    this->_validConfig = false;

    this->_onStartCallback = NULL;
    this->_onReconnectCallback = NULL;

    this->timer = AliotTimer();

    #ifdef ALIOT_DEBUG
        Serial.begin(115200);
    #endif
}

// Dont call this yet, causes corrupted head error
AliotObject::~AliotObject() {
    this->_client.disconnect();
}

void AliotObject::onStart(AliotEventCallback callback) {
    this->_onStartCallback = callback;
}

bool AliotObject::isConnected() {
    return this->_connected;
}

void AliotObject::run() {
    if (this->_validConfig) {
        this->setupWiFi();
        this->setupWebSocket(); 
    } else {
        Serial.println("[Error] Invalid configuration. Call setupConfig() before run()");
    }
}

void AliotObject::runLocal(const char* IPAdress) {
    this->_config.host = IPAdress;
    this->run();
}

void AliotObject::setupConfig(const char* authToken, const char* objectId, const char* ssid, const char* password) {
    this->_config.authToken = authToken;
    this->_config.objectId = objectId;
    this->_config.ssid = ssid;
    this->_config.password = password;

    this->_validConfig = true;
}

void AliotObject::setupConfig(const char* authToken, const char* objectId, const char* ssid, const char* password, const bool modemSleep) {
    setupConfig(authToken, objectId, ssid, password);
    this->_config.modemSleep = modemSleep;
}

void AliotObject::setReconnectCallback(AliotEventCallback callback) {
    this->_onReconnectCallback = callback;
}

void AliotObject::setupWiFi() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(this->_config.ssid, this->_config.password);
    // WiFi.setSleep(this->_config.modemSleep);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
}

// TODO: Switch to secure connection 
void AliotObject::setupWebSocket() {
    #ifdef ALIOT_DEBUG
        Serial.println("[AliotWS] Creating WebSocket");
    #endif
    this->_client.begin(this->_config.host, this->_config.port, this->_config.path);
    this->beginEventListener();
}

void AliotObject::loop() {
    // If the WiFi is disconnected, we reconnect
    if (!WiFi.isConnected()) {
        Serial.println("WiFi disconnected, reconnecting...");

        WiFi.disconnect();

        this->run();

        if (this->_onReconnectCallback != NULL) this->_onReconnectCallback();
    }
    
    this->_client.loop();
}

void AliotObject::runBeforeDeepSleep(AliotTimerCallback timerCallback) {
    while (!this->_connected) 
        this->_client.loop();

    if (timerCallback()) 
        timer.deepSleep();
}

bool AliotObject::sendEvent(AliotEvent_t event, const char* data) {
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
    if (!strcmp(event, AliotEvents::EVT_UPDATE_DOC)) {
        payloadDoc["data"]["fields"] = dataHolderDoc.as<JsonObject>();
    } else { 
        payloadDoc["data"] = dataHolderDoc.as<JsonObject>();
    }   

    // Re-convert the formatted payload to string again
    serializeJson(payloadDoc, output);

    // send payload
    
    #ifdef ALIOT_DEBUG
        Serial.println("[AliotWS] Sent event");
        Serial.println(output);
    #endif
    
    return this->_client.sendTXT(output.c_str());
}

bool AliotObject::sendEvent(AliotEvent_t event, String data) {
    // Exists only in case we forget to call c_str() 
    return this->sendEvent(event, data.c_str());
}

bool AliotObject::updateDoc(AliotDict_t aliotDict) {
    if (this->_connected) 
        return this->sendEvent(AliotEvents::EVT_UPDATE_DOC, aliotDict);
    return false;
}

String AliotObject::getDoc(String key) {
    String path = String(this->_config.apiPath) + "/iot/aliot/" + AliotEvents::EVT_GET_FIELD;
    String contentType = "application/json";
    String postData = "{\"id\": \"" + String(this->_config.objectId) + "\",\"field\":\"" + key + "\"}";

    WiFiClientSecure wifiSecured = WiFiClientSecure();

    wifiSecured.setInsecure(); // NOT RECOMMENDED TODO : add CA certificate

    HttpClient httpClient = HttpClient(wifiSecured, this->_config.host, 443);
    httpClient.post(path, contentType, postData);

    int status = httpClient.responseStatusCode();
    String response = httpClient.responseBody();

    httpClient.stop();
    wifiSecured.stop();

    if (status == 201)  return response;

    #ifdef ALIOT_DEBUG
        switch (status) {
            case 403: Serial.println("While getting the field " + key + ", request was Forbidden due to permission errors or project missing.");
            case 500: Serial.println("While getting the field " + key + ", something went wrong with the ALIVEcode's servers, please try again.");
            default : Serial.println("While getting the field " + key + ", please try again. " + response);
        }
    #endif

    return "";
}

bool AliotObject::connectObject() { 
    #ifdef ALIOT_DEBUG
        Serial.println("[AliotWS] Connecting to WebSocket");
    #endif
    return this->sendEvent(AliotEvents::EVT_CONNECT_OBJECT,
        createDict<const char*, 2>({
            Pair<const char*>("token", this->_config.authToken),
            Pair<const char*>("id", this->_config.objectId)
    }));
}

void AliotObject::onOpen() {
    this->connectObject();
    // onOpen and connectObject are different methods, just in case we'll add sth else to onOpen
    #ifdef ALIOT_DEBUG
        Serial.println("[AliotWS] Connected to WebSocket");
    #endif
    
}

void AliotObject::onClose() {
    #ifdef ALIOT_DEBUG
        Serial.println("[AliotWS] Disconnected");
    #endif
}

void AliotObject::onMessage(uint8_t * payload, size_t length) { 
    #ifdef ALIOT_DEBUG
        Serial.println((char*) payload);
    #endif

    StaticJsonDocument<256> doc;
    deserializeJson(doc, (char*) payload);

    this->handleEvent(doc["event"], doc);
}

void AliotObject::onActionRecv(const char* actionID, AliotActionCallback callback) {
    this->onActionRecv(actionID, callback, true);
}

void AliotObject::onActionRecv(const char* actionID, AliotActionCallback callback, bool logReception) {
    // Create wrapper function to pass to WebSocketClientEvent callback

    auto wrapper = [this, actionID, logReception, callback](const char* data) {
        if (logReception) {
            #ifdef ALIOT_DEBUG
                Serial.print("The protocol: ");
                Serial.print(actionID);
                Serial.print(" was called with the arguments: ");
                Serial.print(data);
                Serial.println();
            #endif
        }

        bool res = false;
        if (callback) {
            res = callback(data);
        }

        return this->sendEvent(AliotEvents::EVT_SEND_ACTION_DONE,
            createDict<const char*, 2>({
                Pair<const char*>("actionId", actionID),
                Pair<const char*>("value",res ? "true" : "false")
        }));
    };

    // Add action to action map
    for (int i = 0; i < MAX_ACTION_COUNT; i++) {
        if (this->_actionMap[i].actionId != NULL) continue;

        this->_actionMap[i] .actionId = (char*) actionID;
        this->_actionMap[i] .callback = wrapper;

        #ifdef ALIOT_DEBUG
            Serial.print("[AliotWS] Added action ");
            Serial.print(actionID);
            Serial.print(" to action map at index ");
            Serial.println(i);
        #endif

        return;
    }

    #ifdef ALIOT_DEBUG
        Serial.println("[AliotWS] Action map is full");
    #endif
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

void AliotObject::handleEvent(AliotEvent_t event, StaticJsonDocument<256> doc) {

    // Reminder : strcmp returns 0 if strings are equal

    if (!strcmp(event, AliotEvents::EVT_PING))  {
        this->sendEvent(AliotEvents::EVT_PONG, "");
    }

    else if (!strcmp(event, AliotEvents::EVT_ERROR)) {
        this->onError(doc["data"]); // Handles only "already connected" error
    }

    else if (!strcmp(event, AliotEvents::EVT_CONNECT_SUCCESS)) {
        this->_connected = true;
        if (_onStartCallback) _onStartCallback();
    }

    else if (!strcmp(event, AliotEvents::EVT_RECEIVE_ACTION)) {
        JsonObject data = doc["data"];

        DynamicJsonDocument dataDoc(256);
        dataDoc["data"] = data;

        // Extract values
        const char* actionID = dataDoc["data"]["id"];
        
        String actionValue = dataDoc["data"]["value"].as<String>();
        const char* actionValueChar = actionValue.c_str();

        // Call action callback
        for (int i = 0; i < MAX_ACTION_COUNT; i++) {
            const char* currentActionID = this->_actionMap[i].actionId;
            if (strcmp(currentActionID, actionID)) continue;

            this->_actionMap[i].callback(actionValueChar);
            
            return;
        }

        #ifdef ALIOT_DEBUG
            Serial.print("[AliotWS] Could not call action ");
            Serial.print(actionID);
            Serial.println(" is not found in action map");
        #endif

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
