#include "Arduino.h"
#include "DHT.h"

#define DHTPIN 26 
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

// Sensoren & Aktoren
const int analogPinBoden = 35;   // Analoger Ausgang Bodenfeuchte
const int fuellstandPin = 18;    // Digitaler Füllstandssensor
const int pumpPin = 12;          // Relais / MOSFET für Pumpe

// Schwellenwerte
const int FEUCHTIGKEIT_EIN = 30;  // Unter 30% -> Pumpe AN
const int FEUCHTIGKEIT_AUS = 40;  // Über 40% -> Pumpe AUS

// Zeitsteuerung
unsigned long letzterLoop = 0;
const unsigned long INTERVALL_LOOP = 1000; // 1 Sekunde Update

bool pumpeAn = false;

void setup() {
  Serial.begin(115200);
  Serial.println("\n🌿 ===== Automatische Bewässerungssteuerung gestartet =====");

  dht.begin();
  pinMode(analogPinBoden, INPUT);
  pinMode(fuellstandPin, INPUT_PULLUP);
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);
}

void loop() {
  unsigned long jetzt = millis();

  if (jetzt - letzterLoop >= INTERVALL_LOOP) {
    letzterLoop = jetzt;

    // 🌱 Bodenfeuchte
    int analogValBoden = analogRead(analogPinBoden);
    float feuchteProzent = map(analogValBoden, 0, 4095, 100, 0);

    // 💧 Füllstand
    int fuellstand = digitalRead(fuellstandPin);
    String tankStatus = (fuellstand == 0) ? "💧 Tank voll" : "⚠️ Tank leer";

    // 🌡️ Temperatur & Luftfeuchtigkeit (immer mitlesen)
    float temp = dht.readTemperature();
    float luft = dht.readHumidity();

    // 🚰 Pumpe steuern anhand Bodenfeuchte
    if (feuchteProzent < FEUCHTIGKEIT_EIN && !pumpeAn) {
      pumpeAn = true;
      digitalWrite(pumpPin, HIGH);
    } else if (feuchteProzent > FEUCHTIGKEIT_AUS && pumpeAn) {
      pumpeAn = false;
      digitalWrite(pumpPin, LOW);
    }

    // 🧾 Ausgabe
    Serial.println("\n------------------------------------------");
    Serial.print("⏱ Laufzeit: ");
    Serial.print(jetzt / 1000.0, 1);
    Serial.println(" s");

    Serial.print("🌱 Bodenfeuchte: ");
    Serial.print(feuchteProzent, 1);
    Serial.println(" %");

    Serial.print("💧 Füllstand: ");
    Serial.println(tankStatus);

    Serial.print("🌡️  Temperatur: ");
    Serial.print(temp);
    Serial.print(" °C   |   💨 Luftfeuchtigkeit: ");
    Serial.print(luft);
    Serial.println(" %");

    Serial.print("🚰 Pumpe: ");
    Serial.println(pumpeAn ? "AN ✅" : "AUS ⛔");
  }
}
