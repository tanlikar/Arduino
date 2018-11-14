#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>
#include "base64.h"

const char* ssid = "DRDMNET";
const char* password = "drdm1234";

String authUsername = "che.junket";
String authPassword = "password";

void setup() {
  // put your setup code here, to run once:
Serial.begin(115200);
WiFi.begin(ssid, password);

while(WiFi.status() != WL_CONNECTED){
  delay(1000);
  Serial.println("Connecting to Wifi...");
}
Serial.println("connected to Wifi");
}

void loop() {
  // put your main code here, to run repeatedly:
if((WiFi.status()==WL_CONNECTED)){
  HTTPClient http;

  http.begin("http://131.1.10.40:3128/auth");

  String auth = base64::encode(authUsername + ":" + authPassword);
  http.addHeader("Authorization", "Basic "+ auth);

  int httpCode = http.GET();

  if(httpCode>0){
    String payload = http.getString();
    Serial.println(httpCode);
    Serial.println(payload);
  }
  else{
    Serial.println("Error on HTTP request");
  }
  http.end();
}
delay(10000);
}
