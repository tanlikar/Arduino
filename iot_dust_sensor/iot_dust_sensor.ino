/*
  MQTT DHT22, when "temperature, c" is sent it returns the temperature in celcius
  when "humidity" is sent it returns the humidity as measured by the DHT22
  the signal pin is connected to a pull-ip resistor and GPIO 2
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

 
#define measurePin  A0
#define ledPower  5
 
int samplingTime = 280;
int deltaTime = 40;
int sleepTime = 9680;
 
float voMeasured = 0;
float calcVoltage = 0;
float dustDensity = 0;

// Update these with values suitable for your network.

const char* ssid = "tan";
const char* password = "123456789";
const char* mqtt_server = "192.168.137.139";
const char* clientID = "NodeMCUDevKit3";

//outtopic esp8266 mqtt output
const char* outTopic1 = "dustOut";
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
    getdust();
    Serial.println(dustDensity);
    dtostrf(dustDensity , 5, 2, msg);

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
  pinMode(ledPower,OUTPUT);

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void getdust(){

digitalWrite(ledPower,LOW); // power on the LED
  delayMicroseconds(samplingTime);
 
  voMeasured = analogRead(measurePin); // read the dust value
 
  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH); // turn the LED off
  delayMicroseconds(sleepTime);
 
  // 0 - 5V mapped to 0 - 1023 integer values
  // recover voltage
  calcVoltage = voMeasured * (5.0 / 1024);
  
 
  // linear eqaution taken from http://www.howmuchsnow.com/arduino/airquality/
  // Chris Nafis (c) 2012
  dustDensity = 0.17 * calcVoltage - 0.1;

  if(dustDensity < 0)
  {
    dustDensity = 0;   
  }
 
  Serial.print("Raw Signal Value (0-1023): ");
  Serial.print(voMeasured);
 
  Serial.print(" - Voltage: ");
  Serial.print(calcVoltage);
 
  Serial.print(" - Dust Density(mg/m^3): ");
  Serial.println(dustDensity);
 
 // delay(1000);
}


