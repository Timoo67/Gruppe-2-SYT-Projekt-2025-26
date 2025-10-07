#include <Arduino.h>
#include <DHT.h>

#define dhtType DHT11
#define dhtPin 32

DHT sensor(dhtPin, dhtType);

// put function declarations here:
// int myFunction(int, int);

void setup() {
  Serial.begin(9600);
  sensor.begin();
  // put your setup code here, to run once:
  // int result = myFunction(2, 3);
}

void loop() {
  float relLuftfeuchtigkeit = sensor.readHumidity();
  float tempC = sensor.readTemperature();

  if(isnan(relLuftfeuchtigkeit) || isnan(tempC))  {
    Serial.println("Fehler beim Lesen der Daten!");
    return;
  }

  Serial.print("rel. Luftfeuchtigkeit: ");
  Serial.print(String(relLuftfeuchtigkeit));
  Serial.print("%");
  Serial.print(" | ");
  Serial.print("Temperature: ");
  Serial.print(String(tempC, 2));
  Serial.println("°C");
  delay(2000);

  // put your main code here, to run repeatedly:
}

// put function definitions here:
// int myFunction(int x, int y) {
//   return x + y;
// }
