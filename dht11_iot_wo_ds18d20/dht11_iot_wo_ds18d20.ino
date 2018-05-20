/*
  MQTT DHT22, when "temperature, c" is sent it returns the temperature in celcius
  when "humidity" is sent it returns the humidity as measured by the DHT22
  the signal pin is connected to a pull-ip resistor and GPIO 2
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

#define DHTTYPE DHT11
#define DHTPIN  2
#define led 14

// Update these with values suitable for your network.

const char* ssid = "tan";
const char* password = "123456789";
const char* mqtt_server = "192.168.137.139";
const char* clientID = "NodeMCUDevKit1";

//outtopic esp8266 mqtt output
const char* outTopic1 = "DHT11out";
const char* outTopic2 = "LEDout";

//intopic esp8266 mqtt input
const char* inTopic1 = "DHT11interval";
const char* inTopic2 = "LEDin";

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
String msg1, temp1, humi1;
char msg[50], temp[50], humi[50];

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
    client.publish(outTopic1, msg);
}
if(strcmp(topic, inTopic2)==0){
  if(message == "1")
    digitalWrite(led, HIGH);
  else if(message == "0")
    digitalWrite(led, LOW);
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
  pinMode(led, OUTPUT);
  digitalWrite(led, LOW);
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  dht.begin();           // initialize temperature sensor

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void gettemperature() {
  // Wait at least 2 seconds seconds between measurements.
  // if the difference between the current time and last time you read
  // the sensor is bigger than the interval you set, read the sensor
  // Works better than delay for things happening elsewhere also
  unsigned long currentMillis = millis();
 
  if(currentMillis - previousMillis >= interval) {
    // save the last time you read the sensor 
    previousMillis = currentMillis;   

    // Reading temperature for humidity takes about 250 milliseconds!
    // Sensor readings may also be up to 2 seconds 'old' (it's a very slow sensor)
    
    humidity = dht.readHumidity();          // Read humidity (percent)
    temp_c = dht.readTemperature();     // Read temperature as Celcius
    // Check if any reads failed and exit early (to try again).
    if (isnan(humidity) || isnan(temp_c)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  }
}

