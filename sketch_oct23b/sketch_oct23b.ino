
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "SparkFunHTU21D.h"

bool debug = true;

HTU21D myHumidity;

#include "Adafruit_SGP30.h"
Adafruit_SGP30 sgp;


uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

// Update these with values suitable for your network.

const char* ssid = "tan";
const char* password = "123456789";
const char* mqtt_server = "192.168.0.100";
const char* clientID = "NodeMCUDevKit";

//outtopic esp8266 mqtt output
const char* outTopic1 = "DHT11out"; //nodemcu send out
const char* outTopic3 = "Sgp30out";

//intopic esp8266 mqtt input
const char* inTopic1 = "DHT11interval"; //nodemcu receive

 // Values read from sensor
float humidity, temp_c; 
String msgHTU, temp, humi;
char cMsgHTU[50], cTemp[50], cHumi[50];

uint16_t mVoc, mCo2e, Voc_base, eCo2_base;
String msgSgp30, tempVoc, tempCo2e, tempVoc_base, tempCo2e_base;
char cMsgSgp30[50];
int counter;

WiFiClient espClient;
PubSubClient client(espClient);

void getTempHumiReading() {

    humidity = myHumidity.readHumidity();          // Read humidity (percent)
    temp_c = myHumidity.readTemperature();    // Read temperature as Celcius

   if(debug){
        Serial.println(temp_c);   
        Serial.println(humidity);
    }
}
void getSgp30Reading(){
  
  sgp.IAQmeasure();
  mVoc = sgp.TVOC;
  mCo2e = sgp.eCO2;
  counter++;
  if(counter ==30)
  sgp.getIAQBaseline(&eCo2_base, &Voc_base);
  
}

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
    getTempHumiReading();

    dtostrf(temp_c , 5, 2, cTemp);
    dtostrf(humidity , 5, 2, cHumi);

    temp = cTemp;
    humi = cHumi;
    msgHTU = temp + " " + humi ;
    msgHTU.toCharArray(cMsgHTU, 50);
    client.publish(outTopic1, cMsgHTU);
}

if(strcmp(topic,inTopic1)==0){ // strcmp return 0 if true
    getSgp30Reading();

    tempVoc = String(mVoc);
    tempCo2e = String(mCo2e);
    tempVoc_base = String(Voc_base);
    tempCo2e_base = String(eCo2_base);

    msgSgp30 = tempVoc + " " + tempCo2e + " " + tempVoc_base + " " + tempCo2e_base;

    msgSgp30.toCharArray(cMsgSgp30, 50);
    client.publish(outTopic3, cMsgSgp30);
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
   myHumidity.begin();           // initialize temperature sensor
  sgp.begin();

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}




