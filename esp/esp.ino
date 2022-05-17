#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Wire.h>
#include <Adafruit_ADS1X15.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <SD.h>

#define STASSID "fffD"
#define STAPSK "12345678"
byte tries = 10;

Adafruit_ADS1115 ads;

void setup() {
  Serial.begin(115200);
  for (uint8_t t = 4; t > 0; t--) {
    Serial.printf("[SETUP] WAIT %d...\n", t);
    Serial.flush();
    delay(1000);
  }
  
  ads.setGain(GAIN_TWO);
  ads.begin();
  WiFi.begin(STASSID, STAPSK);
  while (--tries && WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Non Connecting to WiFi..");
  } else {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void loop() {
  delay(1000);
  int16_t adc_01_voltage;
  adc_01_voltage = ads.readADC_Differential_0_1();
  
  StaticJsonDocument<100> jsonDocument;
  jsonDocument["mac"] = "24:a1:60:30:4c:71";
  jsonDocument["voltage"] = round((float(adc_01_voltage) * 0.0625 / 1000.0)*1000)/1000;
  char buffer[100];
  serializeJsonPretty(jsonDocument, buffer);
  
  if ((WiFi.status() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;
    
    http.begin(client, "http://92.255.110.121:5000/insert");
    http.addHeader("Content-Type", "application/json");
    
    //int httpCode = http.POST("{\"voltage\":\"311.1\", \"ph\": \"8418.1\", \"temp\": \"31.1\", \"mac\": \"24:a1:60:30:4c:71\"}");
    //String req = "{\"mac\": \"24:a1:60:30:4c:71\", \"voltage\":\"311.1\"}";
    
    int httpCode = http.POST(buffer);
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
    
    http.end();
  }
  delay(4000);
}
