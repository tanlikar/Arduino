#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DaikinIRESP8266.h"

DaikinIRESP8266 daikin = DaikinIRESP8266(4);

const char* ssid = "tan";
const char* password = "123456789";
const char* mqtt_server = "broker.hivemq.com";
const char* clientID = "NodeMCUDevKit1";

//intopic esp8266 mqtt input
const char* inTopic1 = "Ac_controller"; //nodemcu receive
const char* inTopic2 = "autoPMV";
const char* inTopic3 = "OnIn";
const char* inTopic4 = "ControlState";

int setPoint;

WiFiClient espClient;
PubSubClient client(espClient);

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

void callback(char* topic, byte* payload, unsigned int length) {
  // Conver the incoming byte array to a string
  payload[length] = '\0'; // Null terminator used to terminate the char array
  String message = (char*)payload;
  
  Serial.print("Message arrived on topic: [");
  Serial.print(topic);// received topic
  Serial.print("], ");
  Serial.println(message); // recived payload

if(strcmp(topic,inTopic1)==0){ // strcmp return 0 if true
  Serial.println((String)inTopic1 + " " + message);
  setPoint = message.toInt();
  for(int x = 0; x<6; x++){
    daikin.setTemp(setPoint);
    daikin.sendComm();
  }
}
if(strcmp(topic,inTopic3)==0){
  Serial.println((String)inTopic3 + " " + message);
  if(message == "1"){
    daikin.setPower(true);
    daikin.sendComm();
  }
}
if(strcmp(topic,inTopic4)==0){
  Serial.println((String)inTopic3 + " " + message);
  if(message == "1"){
    setPoint -= 1;
    for(int x = 0; x<6; x++){
      daikin.setTemp(setPoint);
      daikin.sendComm();
    }
  }
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
      client.subscribe(inTopic2);
      client.subscribe(inTopic3);
      client.subscribe(inTopic4);
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
  // put your setup code here, to run once:
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  daikin.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
   if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
