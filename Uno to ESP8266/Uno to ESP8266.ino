/*  Commenting out the DHT pieces of code to just use a random number for now.
    The delay() code in loop() will prevent constant readings, but may cause issues
    if I want to send instructions back to the UNO, such as turn on fan or pumps.
*/

#include <ArduinoJson.h>
#include "DHT.h"

String message = "";
bool messageReady = false;
//DHT dhtWhite22(DHTPIN, DHTTYPE);
int DHT22UpdateFrequency = 10; // How often we want to poll the DHT22 for temp and humidity in seconds
int soilUpdateFrequency = 10; // How often we want to poll the soil moisture sensors in seconds
unsigned long lastDHT22Update = -DHT22UpdateFrequency;  // Initialize time to -DHT22UpdateFrequency to get readings 1st time through 'if loop'.
unsigned long lastSoilUpdate = -soilUpdateFrequency;  // Initialize time to -soilUpdateFrequency to get readings 1st time through 'if loop'.
float tempF = -100;
float humidity = 0;
float soil1 = 0; // Can this be done better with an array?
float soil2 = 0;
#define SOIL1PIN A1 // Analog input pins for soil moisture sensors. Also do via an array?
#define SOIL2PIN A2
#define DHTPIN 7     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dhtWhite22(DHTPIN, DHTTYPE);

void getDHT(float& tempF, float& humidity) {
  tempF = 50.5;
  humidity++; // Increment this so that I know the # is being passed to ESP8266 for testing.
}

void getSoil(int sensorPin, float& moisture, int airVal, int waterVal) {
  // airVal and waterVal are used to determine 0% and 100% moisture. If same/close for all sensors, this can be a #define instead of passed.
  moisture = 69;
}

void setup() {
  // put your setup code here, to run once:
  //dhtWhite22.begin();
  dhtWhite22.begin();
  Serial.begin(9600);
  delay(5000);
}

void loop() {
  // put your main code here, to run repeatedly:
    DynamicJsonDocument doc(1024); // ArduinoJson version 6+
  doc["type"] = "response";
  // Get data from analog sensors
  //doc["randomValue"] = random(0,500);
  //doc["temperatureF"] = random(-50,120);
  //doc["humidity"] = random(0,100);
  //float hW = dhtWhite22.readHumidity();
  float hW = random(0,100);
  float fW = random(-50,120);
  //float fW = dhtWhite22.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(hW) || isnan(fW)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  getDHT(tempF, humidity);

  doc["temperatureF"] = tempF;
  doc["humidity"] = humidity;
  serializeJson(doc,Serial);
  delay(10000);

/*
  // Only process message if there's one
  if(messageReady) {
    // The only messages we'll parse will be formatted in JSON
    DynamicJsonDocument doc(1024); // ArduinoJson version 6+
    // Attempt to deserialize the message
    DeserializationError error = deserializeJson(doc,message);
    if(error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      messageReady = false;
      return;
    }
    if(doc["type"] == "request") {
      doc["type"] = "response";
      // Get data from analog sensors
      doc["randomValue"] = analogRead(A0);
      doc["temperatureF"] = analogRead(A1);
      serializeJson(doc,Serial);
    }
    messageReady = false;
  }
  */
}
