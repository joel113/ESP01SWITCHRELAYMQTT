#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

extern "C" {
  #include "user_interface.h"
}

const char* ssid = "";
const char* password = "";
char* wifiHostName = "joel-pump";

const char* mqtt_server = "joel-flocke";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.println("Setting up the controller.");
  Serial.begin(115200);
  pinMode(0, OUTPUT);
  digitalWrite(0, 1);
  Serial.println("Starting to establish a connection.");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  wifi_station_set_auto_connect(true);
  wifi_station_set_hostname(wifiHostName);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  delay(10000);
  Serial.print("Using IP address ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      Serial.println("Subscribe to /joel-pumpe/commands");
      client.subscribe("/joel-pumpe/commands");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject((char*)payload);
  int waterOn = root["water"];
  if(waterOn > 0 && waterOn < 100) {
    water(waterOn);
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  delay(100);
}

void water(int t) {
  Serial.print("Water on for ");
  Serial.print(t);
  Serial.println(" seconds.");
  digitalWrite(0,0);
  delay(t * 1000);
  digitalWrite(0,1);
  Serial.println("Water off.");
}
