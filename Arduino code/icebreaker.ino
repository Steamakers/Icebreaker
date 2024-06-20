/*
 * Author: Labaki
 * Project: Icebreaker
 */

// Include Libraries
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <dht11.h>

// Constants
const char* ssid = "";
const char* password = "";
#define seaLevelPressure_hPa 1013.25
#define DHT11PIN 4
String stationCode = "TMP001";
bool logValues = false;

// Objects
Adafruit_BMP085 bmp;
dht11 DHT11;

String serverName = "https://icebreaker.labaki.gr/logSensor.php?sensorId=" + stationCode;
unsigned long timerDelay = 5000;

void setup() {
  Serial.begin(115200); 

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
  
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085 sensor, check wiring!");
    while (1) {}
  } else {
    Serial.println("BMP085 connected!");
  }
}

void loop() {
  if(WiFi.status()== WL_CONNECTED and logValues){
    HTTPClient http;
    
    int chk = DHT11.read(DHT11PIN);
    
    String serverPath = serverName + "&dhtTemperature=" + (String)DHT11.temperature
    + "&dhtHumidity=" + (String)DHT11.humidity
    + "&bmpTemperature=" + (String)bmp.readTemperature()
    + "&bmpPressure=" + (String)bmp.readPressure()
    + "&bmpAltitude=" + (String)bmp.readAltitude()
    + "&bmpRealAltitude=" + (String)(bmp.readAltitude(seaLevelPressure_hPa * 100));

    
    // Your Domain name with URL path or IP address with path
    http.begin(serverPath.c_str());
    int httpResponseCode = http.GET();
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
    delay(3000);
  } else {
    int chk = DHT11.read(DHT11PIN);
    Serial.println("Temperature: " + (String)DHT11.temperature);
    Serial.println("Humidity: " + (String)DHT11.humidity);
    Serial.println("BMP temp: " + (String)bmp.readTemperature());
    Serial.println("Pressure: " + (String)bmp.readPressure());
    Serial.println("Altitude: " + (String)bmp.readAltitude());
    Serial.println("Real altitude: " + (String)(bmp.readAltitude(seaLevelPressure_hPa * 100)));
    Serial.println("Moisture: " + (String)map(analogRead(A0), 4095, 1600, 0, 100));
    Serial.println("UV: " + (String) analogRead(A3));
  
    delay(1000);
  }  
}
