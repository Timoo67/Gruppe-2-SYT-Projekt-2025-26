#include <OneWire.h>
#include <DallasTemperature.h>

// Pin, an dem der Datenpin (DQ / OUT) angeschlossen ist
#define ONE_WIRE_BUS 32

// OneWire-Instanz erzeugen
OneWire oneWire(ONE_WIRE_BUS);

// DallasTemperature-Bibliothek mit der OneWire-Instanz verbinden
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  sensors.begin();
  Serial.println("DS18B20 Test gestartet");
}

void loop() {
  // Temperaturmessung anfordern (alle Sensoren am Bus)
  sensors.requestTemperatures();

  // Temperatur des ersten (und einzigen) Sensors lesen
  float tempC = sensors.getTempCByIndex(0);

  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Fehler: Sensor nicht verbunden");
  } else {
    Serial.print("Temperatur: ");
    Serial.print(tempC);
    Serial.println(" °C");
  }

  delay(1000);
}
