/*
This code makes a single switch in Home Assistant that turns on/off the LED on the ESP8266. Also making a Number Sensor.
*/
#include <ESP8266WiFi.h>
#include <ArduinoHA.h>
#include <ArduinoJson.h>
#include "credentials.h" // A file defining WIFI_SSID and WIFI_PASSWORD

WiFiClient client;  //Creates a wifi client to pass to the mqtt broker. Why is this NOT used for the program's wifi connection?
HADevice device("testDevice1"); //Creates a device to pass to the mqtt broker.
HAMqtt mqtt(client, device);  //Holds all HA 'devices' with a limit of 6. I'm only using 1 in this example.
HASwitch led("led");  // "led" is unique ID of the switch. You should define your own ID. Creates a toggle switch type in HA.
HASensorNumber somenum("someNum");  //Creates a sensor that uses a number instead of text. Formatting of number done later.

int updateFrequency = 10; // How often we want to send a broker message to HA in seconds.
unsigned long lastUpdateAt = -updateFrequency; // Initialize time to -updateFrequency to get readings 1st time through 'if loop'.
int randomValue = 0;
float temperatureF = 0;

void onSwitchCommand(bool state, HASwitch* sender)
{
    digitalWrite(LED_BUILTIN, (state ? HIGH : LOW));
    sender->setState(state); // report state back to the Home Assistant
}
void setup() {
    Serial.begin(9600);
    Serial.println("Starting...");
    // Unique ID must be set!
    //byte mac[WL_MAC_ADDR_LENGTH];
    //WiFi.macAddress(mac);
    //device.setUniqueId(mac, sizeof(mac));
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    // connect to wifi
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    delay(7000); //Give me time to turn on serial monitor in IDE.
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500); // waiting for the connection
    }
    Serial.println();
    Serial.println("Connected to the network");
    led.onCommand(onSwitchCommand);     // handle switch state
    led.setName("My LED");      // set switch's details (optional)
    somenum.setName("Some # from ESP8266"); // set number sensor's details
    somenum.setDeviceClass("humidity"); // this sets icon
    somenum.setUnitOfMeasurement("%");  // this sets unit of measurement as well as graphing functionality I believe
    somenum.setForceUpdate(true); // See if this forces HA panel update. 
    mqtt.begin(HA_HOSTNAME, HA_USERNAME, HA_PASSWORD); // starts talking to the HA broker

}
void loop() {
  boolean messageReady = false;
  String message = "";
  // Monitor serial communication
  while(Serial.available()) {
    message = Serial.readString();
    messageReady = true;
  }
  // Only process message if there's one
  if(messageReady) {
    // The only messages we'll parse will be formatted in JSON
    DynamicJsonDocument doc(1024); // ArduinoJson version 6+
    // Attempt to deserialize the JSON-formatted message
    DeserializationError error = deserializeJson(doc,message);
    if(error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.print(error.c_str());
      Serial.print(F("Message was:"));
      Serial.println(message);
      return;
    }
    randomValue = doc["humidity"];
    temperatureF = doc["temperatureF"];
    Serial.print(F("\nIn ESP randomValue="));
    Serial.print(randomValue);
    Serial.print(F("\nIn ESP temperatureF="));
    Serial.println(temperatureF);
    //delay(3000); //No delay needed IF not sending requests.
  }

    mqtt.loop();
    // The following if statement makes effectively a 'delay' for everything inside it. It works better thatn delay() 
    // because it is non-blocking.,
    if ((millis() - lastUpdateAt) > 1000*updateFrequency) { // update in updateFrequency seconds interval.
      unsigned long uptimeValue = millis() / 1000;  // can use uptimeValue variable to determine last boot if wanted.
      //float randNum = random(0,3);  // generates a random # to update HA. Will need to replace with sensor readings.
      Serial.print("Num = ");
      //Serial.println(randNum);
      Serial.println(randomValue);
      //somenum.setValue(randNum,true); // try forcing the mqtt update. 
      somenum.setValue(randomValue,true); // try forcing the mqtt update. 
      lastUpdateAt = millis();
    }
    // You can also change the state at runtime as shown below.
    // This kind of logic can be used if you want to control your switch using a button connected to the device.
    // led.setState(true); // use any state you want
}
