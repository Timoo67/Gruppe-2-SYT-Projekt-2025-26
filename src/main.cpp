#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"

#define DHTPIN 26
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Sensoren & Aktoren
const int analogPinBoden = 35;   // Analoger Ausgang Bodenfeuchte
const int fuellstandPin = 18;    // Digitaler Füllstandssensor
const int pumpPin = 12;          // Relais für Pumpe

const int FEUCHTIGKEIT_EIN = 30;
const int FEUCHTIGKEIT_AUS = 40;

bool pumpeAn = false;
WebServer server(80);

const char* ssid = "ESP32-Bewaesserung";
const char* password = "JosefSinger1";

String getPage(float feuchte, float temp, float luft, String tank, bool pumpe) {
  String html = R"rawliteral(
<!DOCTYPE html><html><head>
<meta charset="UTF-8">
<title>🌿 Smart Garden</title>
<style>
body {font-family: 'Segoe UI', sans-serif; background: linear-gradient(135deg,#a8edea,#fed6e3);
text-align:center; color:#333; margin:0;}
.card {background:white; border-radius:20px; padding:20px; margin:20px auto; width:300px;
box-shadow:0 10px 20px rgba(0,0,0,0.15);}
h1 {font-size:2em;}
.data {font-size:1.5em; margin:10px 0;}
.status {padding:10px; border-radius:10px;}
.on {background:#4CAF50; color:white;}
.off {background:#F44336; color:white;}
</style></head><body>
<h1>🌿 Automatische Bewässerung</h1>
<div class='card'>
  <div class='data'>🌱 Bodenfeuchte: <b>)rawliteral" + String(feuchte, 1) + R"rawliteral( %</b></div>
  <div class='data'>💧 Füllstand: <b>)rawliteral" + tank + R"rawliteral(</b></div>
  <div class='data'>🌡️ Temperatur: <b>)rawliteral" + String(temp, 1) + " °C</b><br>💨 Luftfeuchtigkeit: <b>" + String(luft, 1) + R"rawliteral( %</b></div>
  <div class='data status )rawliteral" + (pumpe ? "on" : "off") + R"rawliteral('>
  🚰 Pumpe: <b>)rawliteral" + (pumpe ? "AN ✅" : "AUS ⛔") + R"rawliteral(</b></div>
</div>
<script>
setTimeout(()=>{location.reload();},2000);
</script></body></html>
)rawliteral";
  return html;
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(analogPinBoden, INPUT);
  pinMode(fuellstandPin, INPUT_PULLUP);
  pinMode(pumpPin, OUTPUT);

  WiFi.softAP(ssid, password);
  Serial.println("WLAN gestartet: " + String(ssid));
  Serial.println(WiFi.softAPIP());

  server.on("/", []() {
    int analogValBoden = analogRead(analogPinBoden);
    float feuchte = map(analogValBoden, 0, 4095, 100, 0);
    int fuellstand = digitalRead(fuellstandPin);
    String tank = (fuellstand == 0) ? "💧 Voll" : "⚠️ Leer";
    float temp = dht.readTemperature();
    float luft = dht.readHumidity();

    if (feuchte < FEUCHTIGKEIT_EIN && !pumpeAn) {
      pumpeAn = true;
      digitalWrite(pumpPin, HIGH);
    } else if (feuchte > FEUCHTIGKEIT_AUS && pumpeAn) {
      pumpeAn = false;
      digitalWrite(pumpPin, LOW);
    }

    server.send(200, "text/html", getPage(feuchte, temp, luft, tank, pumpeAn));
  });

  server.begin();
}

void loop() {
  server.handleClient();
}
