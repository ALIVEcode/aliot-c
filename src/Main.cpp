#include <Arduino.h>
#include <WebSocketsClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>


// WiFi credentials
const char* ssid = "";
const char* password = "";

// Websocket credentials
WebSocketsClient webSocket;

const char* host = "alivecode.ca";
const char* path = "/iotgateway/";
const int port = 8881;

void setupWifi() {
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length)
{
	switch (type)
	{
	case WStype_DISCONNECTED:
		Serial.printf("[WSc] Disconnected!\n");
		break;
	case WStype_CONNECTED:
	{
		Serial.printf("[WSc] Connected to url: %s\n", payload);
		webSocket.sendTXT(
				R"({"event": "connect_object", "data": { "id": "9a38c8ff-45f6-49dd-b38b-0cd1d0fdf981", "token":"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJuYW1lIjoiRGVmYXVsdCBBdXRoVG9rZW4iLCJvYmplY3RJZCI6IjlhMzhjOGZmLTQ1ZjYtNDlkZC1iMzhiLTBjZDFkMGZkZjk4MSIsImlhdCI6MTY3OTUwODIwMywiZXhwIjo0ODMzMTA4MjAzfQ.wQXHcyV-aHhuQdOLntp4kk1TPORsasa5Gp4DAoXsvQk"}})");
		
		break;
	}

	}
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  setupWifi();
  webSocket.begin(host, port, path);
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();


}
