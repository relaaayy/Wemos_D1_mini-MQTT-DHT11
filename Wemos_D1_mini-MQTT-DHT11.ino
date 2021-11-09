// Temperature and Humidity Monitoring with MQTT
// Author: relaychris
// Github: https://github.com/relaychris/ 
// Version: 0.1

//  ######   #######  ####       ##     ##  ##     ####   ##   ##  ######    ####     #####
//   ##  ##   ##   #   ##       ####    ##  ##    ##  ##  ##   ##   ##  ##    ##     ##   ##
//   ##  ##   ## #     ##      ##  ##   ##  ##   ##       ##   ##   ##  ##    ##     #
//   #####    ####     ##      ##  ##    ####    ##       #######   #####     ##      #####
//   ## ##    ## #     ##   #  ######     ##     ##       ##   ##   ## ##     ##          ##
//   ##  ##   ##   #   ##  ##  ##  ##     ##      ##  ##  ##   ##   ##  ##    ##     ##   ##
//  #### ##  #######  #######  ##  ##    ####      ####   ##   ##  #### ##   ####     #####

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include "credentials.h"

#define humidity_topic "sensor/humidity"
#define temp_topic "sensor/temp"

#define dht_type DHT11
//DHT11 Pin to Port D1
#define dht_pin 5


DHT dht(dht_pin, dht_type, 5);
 
float humidity, temp; // Values read from sensor
unsigned long previousMillis = 0;        
const long interval = 10000; // interval at which to read sensor in seconds

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  dht.begin();           // initialize temperature sensor
  setup_wifi();          // connect to wifi
  client.setServer(mqtt_server, 1883); // set mqtt server and port

}

void setup_wifi() {
  delay(10);
  // connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

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

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // Read the sensor
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= interval) {
    // save the last time you read the sensor 
    previousMillis = currentMillis;
    gettemperature();
    String message = String(humidity);
    Serial.print("New Humidity:");
    Serial.println(String(message).c_str());
    client.publish(humidity_topic, String(message).c_str(), true);
    message = String(temp);
    Serial.print("New Temp C:");
    Serial.println(String(message).c_str());
    client.publish(temp_topic, String(message).c_str(), true);
  }
}

void gettemperature() {
  humidity = dht.readHumidity();    // Read humidity (percent)
  temp = dht.readTemperature();     // Read temperature as Fahrenheit
  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temp)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
}
