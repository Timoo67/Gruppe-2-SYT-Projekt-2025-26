#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"
#include <PubSubClient.h>

// ---------- SENSOR & AKTOREN ----------
#define DHTPIN 26
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int analogPinBoden = 35; // Bodenfeuchte
const int fuellstandPin = 18;  // Füllstand
const int pumpPin = 12;        // Pumpe

const int FEUCHTIGKEIT_EIN = 30;
const int FEUCHTIGKEIT_AUS = 40;

bool pumpeAn = false;

// ---------- WLAN ----------
const char* ssid = "NVS-Europa";
const char* password = "nvsrocks";

// ---------- MQTT ----------
const char* mqttServer = "172.16.93.132";
const int mqttPort = 1883;

const char* topicSoil = "test9988/soil";
const char* topicTemp = "test9988/temp";
const char* topicHum = "test9988/humidity";
const char* topicPump = "test9988/pump/state";

WiFiClient espClient;
PubSubClient client(espClient);

// ---------- MQTT Funktionen ----------
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Verbinde mit MQTT...");
    if (client.connect("ESP32Client", NULL, NULL)) {
      Serial.println(" ✅ verbunden!");
    } else {
      Serial.print("❌ Fehler rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);
  dht.begin();

  pinMode(analogPinBoden, INPUT);
  pinMode(fuellstandPin, INPUT_PULLUP);
  pinMode(pumpPin, OUTPUT);

  // WLAN STA
  WiFi.begin(ssid, password);
  Serial.print("Verbinde mit WLAN...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WLAN verbunden!");
  Serial.print("IP-Adresse: ");
  Serial.println(WiFi.localIP());

  // MQTT
  client.setServer(mqttServer, mqttPort);
}

// ---------- LOOP ----------
unsigned long lastSend = 0;

void loop() {
  if (!client.connected()) reconnectMQTT();
  client.loop();

  // Automatisches Senden alle 2 Sekunden
  if (millis() - lastSend > 2000) {
    lastSend = millis();

    int analogValBoden = analogRead(analogPinBoden);
    float feuchte = map(analogValBoden, 0, 4095, 100, 0);
    float temp = dht.readTemperature();
    float luft = dht.readHumidity();
    int fuellstand = digitalRead(fuellstandPin);
    String tank = (fuellstand == 0) ? "💧 Voll" : "⚠️ Leer";

    // Pumpensteuerung
    if (feuchte < FEUCHTIGKEIT_EIN && !pumpeAn) {
      pumpeAn = true;
      digitalWrite(pumpPin, HIGH);
    } else if (feuchte > FEUCHTIGKEIT_AUS && pumpeAn) {
      pumpeAn = false;
      digitalWrite(pumpPin, LOW);
    }

    // MQTT senden
    client.publish(topicSoil, String(feuchte).c_str(), true);
    client.publish(topicTemp, String(temp).c_str(), true);
    client.publish(topicHum, String(luft).c_str(), true);
    client.publish(topicPump, pumpeAn ? "on" : "off", true);

    Serial.println("MQTT Daten gesendet");
  }
}
