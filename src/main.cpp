#include "Arduino.h"
#include "DHT.h"

#define DHTPIN 26 
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const int digitalPinBoden = 14;
const int analogPinBoden = 35;

void setup() {
Serial.begin(115200);
Serial.println(F("DHTxx test!"));
dht.begin();
pinMode(digitalPinBoden, INPUT);
}
void loop() {
  // Wait a few seconds between measurements.
  delay(2000);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
  Serial.println(F("Failed to read from DHT sensor!"));
  }

  Serial.print(F("\n\nHumidity: "));
  Serial.print(h);
  Serial.print(F("% \nTemperature: "));
  Serial.print(t);
  Serial.print(F("°C "));

  int analogValBoden = analogRead(analogPinBoden);
  int digitalValBoden = digitalRead(digitalPinBoden);

  float moisturePercent = map(analogValBoden, 0, 4095, 100, 0);

  Serial.print("\n\nAnalog: ");
  Serial.print(analogValBoden);
  Serial.print(" Feuchte ≈ ");
  Serial.print(moisturePercent);
  Serial.print("%  \nDigital: ");
  Serial.println(digitalValBoden);

  delay(1000);

  return;
}
