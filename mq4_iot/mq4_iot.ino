/*
  MQTT DHT22, when "temperature, c" is sent it returns the temperature in celcius
  when "humidity" is sent it returns the humidity as measured by the DHT22
  the signal pin is connected to a pull-ip resistor and GPIO 2
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "MQ4.h"
 

#define PIN_MQ4 A0
MQ4 mq4_sensor = MQ4(PIN_MQ4, true);


float resistance ;
float ppm ;
float voltage;


// Update these with values suitable for your network.

const char* ssid = "tan";
const char* password = "123456789";
const char* mqtt_server = "192.168.137.139";
const char* clientID = "NodeMCUDevKit5";

//outtopic esp8266 mqtt output
const char* outTopic1 = "ch4ppm";
const char* inTopic1 = "DHT11interval";


WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
//String msg1;

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
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

//mqtt send
void callback(char* topic, byte* payload, unsigned int length) {
  // Conver the incoming byte array to a string
  payload[length] = '\0'; // Null terminator used to terminate the char array
  String message = (char*)payload;
  
  Serial.print("Message arrived on topic: [");
  Serial.print(topic);// received topic
  Serial.print("], ");
  Serial.println(message); // recived payload

  if(strcmp(topic,inTopic1)==0){ // strcmp return 0 if true
    getppm();
    Serial.print("\t CH4 PPM: ");
    Serial.print(ppm);
    Serial.println("ppm");
    dtostrf(ppm , 5, 2, msg);

    //msg1.toCharArray(msg, 50);
    client.publish(outTopic1, msg);
  }
 
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(clientID)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish(outTopic, clientID);
      // ... and resubscribe
      client.subscribe(inTopic1);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  mq4_sensor.begin();

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void getppm(){
  ppm = mq4_sensor.getPPM();

}


