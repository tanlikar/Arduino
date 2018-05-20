/*
  MQTT DHT22, when "temperature, c" is sent it returns the temperature in celcius
  when "humidity" is sent it returns the humidity as measured by the DHT22
  the signal pin is connected to a pull-ip resistor and GPIO 2
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <MQ2Lib.h>


int pin = A0; //change this to the pin that you use
float lpg = 0, co = 0, smoke = 0;

MQ2 mq2(pin, true); //instance (true=with serial output enabled)


// Update these with values suitable for your network.

const char* ssid = "tan";
const char* password = "123456789";
const char* mqtt_server = "192.168.137.139";
const char* clientID = "NodeMCUDevKit3";

//outtopic esp8266 mqtt output
const char* outTopic1 = "MQ2ppmOut";
const char* inTopic1 = "DHT11interval";

WiFiClient espClient;
PubSubClient client(espClient);
char msg[50], lpg1[50], co1[50], smoke1[50];
String msg1, temp1, temp2, temp3;

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
    getMQ2ppm();
    dtostrf(lpg , 5, 2, lpg1);
    dtostrf(co , 5, 2, co1);
    dtostrf(smoke , 5, 2, smoke1);

    temp1 = lpg1;
    temp2 = co1;
    temp3 = smoke1;

    msg1 = temp1 + " " + temp2 + " " + temp3;
    
    msg1.toCharArray(msg, 50);
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
  mq2.begin();
}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void getMQ2ppm (){

/* Read all values from the sensor - it returns an array which contains 3 values
  * 1 = LPG in ppm
  * 2 = CO in ppm
  * 3 = SMOKE in ppm
  */
float* values= mq2.read(true); //set it false if you don't want to print the values in the Serial

  //Reading specific values:
  //lpg = values[0];
  lpg = mq2.readLPG();
  //co = values[1];
  co = mq2.readCO();
  //smoke = values[2];
  smoke = mq2.readSmoke();
  
}


