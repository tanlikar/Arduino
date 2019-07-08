/*
  MQTT DHT22, when "temperature, c" is sent it returns the temperature in celcius
  when "humidity" is sent it returns the humidity as measured by the DHT22
  the signal pin is connected to a pull-ip resistor and GPIO 2
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTTYPE DHT11
#define DHTPIN  14
#define SOIL A0
#define RELAY 5

int AnalogInput = 0;
int threshold = 30;
bool pumpFlag = false;

// Update these with values suitable for your network.

const char* ssid = "tan";
const char* password = "123456789";
const char* mqtt_server = "192.168.137.158";
const char* clientID = "NodeMCUDevKit1";

//outtopic esp8266 mqtt output
const char* outTopic1 = "DHT11out";
const char* outTopic2 = "SOIL";
const char* outTopic3 = "MoistureState";

//intopic esp8266 mqtt input
const char* inTopic1 = "DHT11interval";
const char* inTopic2 = "PumpState";
const char* inTopic3 = "MoistureThreshold";

// Initialize DHT sensor 
// NOTE: For working with a faster than ATmega328p 16 MHz Arduino chip, like an ESP8266,
// you need to increase the threshold for cycle counts considered a 1 or 0.
// You can do this by passing a 3rd parameter for this threshold.  It's a bit
// of fiddling to find the right value, but in general the faster the CPU the
// higher the value.  The default for a 16mhz AVR is a value of 6.  For an
// Arduino Due that runs at 84mhz a value of 30 works.
// This is for the ESP8266 processor on ESP-01 
DHT dht(DHTPIN, DHTTYPE, 11); // 11 works fine for ESP8266

float humidity, temp_c;  // Values read from sensor

// Generally, you should use "unsigned long" for variables that hold time
unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 1000;              // interval at which to read sensor
 
WiFiClient espClient;
PubSubClient client(espClient);
String msg1, temp1, humi1, msg4;
char msg[50], temp[50], humi[50], msgSoil[50];

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
    gettemperature();
    Serial.println(temp_c);
    dtostrf(temp_c , 5, 2, temp);
    Serial.println(humidity);
    dtostrf(humidity , 5, 2, humi);

    temp1 = temp;
    humi1 = humi;
    msg1 = temp1 + " " + humi1 ;

    msg1.toCharArray(msg, 50);
    msg4 = (String)AnalogInput;
    msg4.toCharArray(msgSoil,50);
    client.publish(outTopic1, msg);
    client.publish(outTopic2, msgSoil);
    //Serial.print(msgSoil);

    
}

if(strcmp(topic,inTopic2)==0){ // strcmp return 0 if true
    pumpFlag = true;
}

if(strcmp(topic,inTopic3)==0){ // strcmp return 0 if true
    threshold = message.toInt();
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
  dht.begin();           // initialize temperature sensor
  pinMode(RELAY, OUTPUT);

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

   unsigned long currentMillis = millis();
   if(currentMillis - previousMillis >= interval) {
     previousMillis = currentMillis;   
     AnalogInput = analogRead(SOIL);
     //Serial.print(AnalogInput);
     AnalogInput = map(AnalogInput,1024,398,0,100); //range need to change
     Serial.print("MOisture reading: ");
     Serial.println(AnalogInput);

     if(AnalogInput >= threshold){
      pumpFlag = false;
      digitalWrite(RELAY, LOW);
     }

     if(AnalogInput < threshold){
      digitalWrite(RELAY, HIGH);
      pumpFlag = true;
     }
     Serial.println(pumpFlag);

    /* if(pumpFlag == true){
      digitalWrite(RELAY, HIGH);
     }else{
      digitalWrite(RELAY, LOW);
     }*/
     //TODO water now button is wrong, need add another flag for water now button, add another threshold for pump (80%)
     
     
}
}

void gettemperature() {
  // Wait at least 2 seconds seconds between measurements.
  // if the difference between the current time and last time you read
  // the sensor is bigger than the interval you set, read the sensor
  // Works better than delay for things happening elsewhere also
 // unsigned long currentMillis = millis();
 
  //if(currentMillis - previousMillis >= interval) {
    // save the last time you read the sensor 
   // previousMillis = currentMillis;   

    // Reading temperature for humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    
    humidity = dht.readHumidity();          // Read humidity (percent)
    temp_c = dht.readTemperature();     // Read temperature as Celcius
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temp_c)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  //}
}
