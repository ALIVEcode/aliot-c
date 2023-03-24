#include <Arduino.h>
#include <WebSocketsClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include "AliotWebSocket.h"


// WiFi credentials
const char* ssid = "TransatTelecom19571";
const char* password = "54a17fd4";

AliotWebSocket AWSClient = AliotWebSocket();

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

void setup() {
  Serial.begin(115200);
  pinMode(0, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);
  setupWifi();
  AWSClient.run();
  AWSClient.event();
}

void loop() {
  if (digitalRead(0) == LOW) {
    // Disconnect when button is pressed
    AWSClient.~AliotWebSocket();
  }

}
