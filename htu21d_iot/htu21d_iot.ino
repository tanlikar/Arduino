/*
  MQTT DHT22, when "temperature, c" is sent it returns the temperature in celcius
  when "humidity" is sent it returns the humidity as measured by the DHT22
  the signal pin is connected to a pull-ip resistor and GPIO 2
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "SparkFunHTU21D.h"

#include <pm25.h>
#include <SoftwareSerial.h>

bool debug = false;

HTU21D myHumidity;
SoftwareSerial pm25_ser(1, 3);

#include "Adafruit_SGP30.h"
Adafruit_SGP30 sgp;

#include "T6713.h"
T6713 t6713;

void PM25_listen(){
  if(!pm25_ser.isListening()){
    Serial.println(F("[PM25] >> Listening"));
    pm25_ser.listen();
  }
}

uint32_t getAbsoluteHumidity(float temperature, float humidity) {
    // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
    const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
    const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
    return absoluteHumidityScaled;
}

// Update these with values suitable for your network.

const char* ssid = "tan";
const char* password = "123456789";
const char* mqtt_server = "192.168.137.139";
const char* clientID = "NodeMCUDevKit";

//outtopic esp8266 mqtt output
const char* outTopic1 = "DHT11out"; //nodemcu send out
const char* outTopic2 = "PMout";
const char* outTopic3 = "Sgp30out";

//intopic esp8266 mqtt input
const char* inTopic1 = "DHT11interval"; //nodemcu receive

 // Values read from sensor
float humidity, temp_c; 
String msgHTU, temp, humi;
char cMsgHTU[50], cTemp[50], cHumi[50];

uint16_t mPM25,mPM10;
String msgPM, tempPM25, tempPM10;
char cMsgPM[50];

uint16_t mVoc, mCo2e, Voc_base, eCo2_base;
String msgSgp30, tempVoc, tempCo2e, tempVoc_base, tempCo2e_base;
char cMsmSgp30[50];
int counter;

int Co2;
String cMsgCo2[50];

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
    getPmReading();

    tempPM25 = String(mPM25);
    tempPM10 = String(mPM10);

    msgPM = tempPM25 + " " + tempPM10;

    msgPM.toCharArray(cMsgPM, 50);
    client.publish(outTopic2, cMsgPM);
}

if(strcmp(topic,inTopic1)==0){ // strcmp return 0 if true
    getSgp30Reading();

    tempVoc = String(mVoc);
    tempCo2e = String(mCo2e);
    tempVoc_base = String(Voc_base);
    tempCo2e_base = String(eCo2_base);

    msgSgp30 = tempVoc + " " + tempCo2e + " " + tempVoc_base + " " + tempCo2e_base;

    msgSgp30.toCharArray(cMsmSgp30, 50);
    client.publish(outTopic3, cMsmSgp30);
}

if(strcmp(topic,inTopic1)==0){ // strcmp return 0 if true
    Co2=t6713.readCO2();
    
    msgSgp30 = tempVoc + " " + tempCo2e + " " + tempVoc_base + " " + tempCo2e_base;

    msgSgp30.toCharArray(cMsmSgp30, 50);
    client.publish(outTopic3, cMsmSgp30);
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

  pm25_ser.begin(9600);
  // set uart for pm25, and uart for debug
  // set listen function if one or more SoftwareSerials are used
  PM25.init(&pm25_ser, &Serial, PM25_listen);
  
  sgp.begin();
  
  t6713.begin();

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}

void getTempHumiReading() {

    humidity = myHumidity.readHumidity();          // Read humidity (percent)
    temp_c = myHumidity.readTemperature();    // Read temperature as Celcius

   if(debug){
        Serial.println(temp_c);   
        Serial.println(humidity);
    }
}

void getPmReading(){

   mPM25 = PM25.read();
   mPM10 = PM25.read(PM10_TYPE);
   
   if(debug){
      Serial.print(F("PM2.5 val: ")); 
      Serial.print(mPM25);
      Serial.println(F("  ug/m3"));
      Serial.print(F("PM2.5 val: ")); 
      Serial.print(mPM10);
      Serial.println(F("  ug/m3"));
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




