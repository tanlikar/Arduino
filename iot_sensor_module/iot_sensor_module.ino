/*
  MQTT DHT22, when "temperature, c" is sent it returns the temperature in celcius
  when "humidity" is sent it returns the humidity as measured by the DHT22
  the signal pin is connected to a pull-ip resistor and GPIO 2
*/
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include "SparkFunHTU21D.h"
#include <EEPROM.h>
#include <pm25.h>
#include <SoftwareSerial.h>

bool debug = true;

HTU21D myHumidity;
SoftwareSerial pm25_ser(D7, D8);

#include "Adafruit_SGP30.h"
Adafruit_SGP30 sgp;

#define T6713_Address 0x15 //T6713 i2C Address
byte data[6];
int CO2ppmValue;
const int cmdReadCo2[] = {0x04, 0x13, 0x8B, 0x00, 0x01};

int period = 1000;
unsigned long time_now = 0;

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
const char* mqtt_server = "broker.hivemq.com";
const char* clientID = "NodeMCUDevKit";

//outtopic esp8266 mqtt output
const char* outTopic1 = "DHT11out"; //nodemcu send out
const char* outTopic2 = "PMout";
const char* outTopic3 = "Sgp30out";
const char* outTopic4 = "T6713out";
const char* outTopic5 = "Sgp30baseline";

//intopic esp8266 mqtt input
const char* inTopic1 = "DHT11interval"; //nodemcu receive
const char* inTopic2 = "SGPbaseline";

 // Values read from sensor
float humidity, temp_c; 
String msgHTU, temp, humi;
char cMsgHTU[50], cTemp[50], cHumi[50];

uint16_t mPM25,mPM10;
String msgPM, tempPM25, tempPM10;
char cMsgPM[50];

uint16_t mVoc, mCo2e, Voc_base, eCo2_base;
String msgSgp30, tempVoc, tempCo2e, tempVoc_base, tempCo2e_base;
char cMsgSgp30[50];

String msgSgp30base;
char cMsgSgp30base[50];

int Co2;
String msgCo2;
char cMsgCo2[50];

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
  
    tempPM25 = String(mPM25);
    tempPM10 = String(mPM10);

    msgPM = tempPM25 + " " + tempPM10;

    msgPM.toCharArray(cMsgPM, 50);
    client.publish(outTopic2, cMsgPM);
}

if(strcmp(topic,inTopic1)==0){ // strcmp return 0 if true
   
    tempVoc = String(mVoc);
    tempCo2e = String(mCo2e);

    msgSgp30 = tempVoc + " " + tempCo2e;
    msgSgp30.toCharArray(cMsgSgp30, 50);
    client.publish(outTopic3, cMsgSgp30);
}

if(strcmp(topic,inTopic1)==0){ // strcmp return 0 if true
    GetCO2PPM();
    msgCo2 = CO2ppmValue;
    
    msgCo2.toCharArray(cMsgCo2, 50);
    client.publish(outTopic4, cMsgCo2);
}

if(strcmp(topic,inTopic2)==0){

    sgp30SetBase();
    tempVoc_base = String(Voc_base);
    tempCo2e_base = String(eCo2_base);

    msgSgp30base = tempVoc_base + " " + tempCo2e_base;
    msgSgp30base.toCharArray(cMsgSgp30base, 50);
    client.publish(outTopic5, cMsgSgp30base);
    
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
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
   myHumidity.begin();           // initialize temperature sensor

  pm25_ser.begin(9600);
  // set uart for pm25, and uart for debug
  // set listen function if one or more SoftwareSerials are used
  PM25.init(&pm25_ser, &Serial, PM25_listen);
    // enable auto send, allows auto sampling at 1s interval
  PM25.enableAutoSend();
  
  sgp.begin();
  sgp.IAQinit();
  uint16_t initVOC = EEPROMRead(2);
  uint16_t initCo2e = EEPROMRead(0);
  sgp.setIAQBaseline(initCo2e, initVOC); 
  

}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if(millis()> time_now + period){
    time_now = millis();
     getSgp30Reading();

    if(PM25.loop()){
    mPM25 = PM25.get();
    mPM10 = PM25.get(PM10_TYPE);
    Serial.print("PM2.5 val: ");
    Serial.print(mPM25);
    Serial.println("  ug/m3");
    Serial.print("PM10 val: ");
    Serial.print(mPM10);
    Serial.println("  ug/m3");
  }
  }

    
}

void getTempHumiReading() {

    humidity = myHumidity.readHumidity();          // Read humidity (percent)
    temp_c = myHumidity.readTemperature();    // Read temperature as Celcius

    if(humidity > 100 )
    {
          humidity = 0;
    }
    if(temp_c>100){
      temp_c = 0;
    }
    
   if(debug){
        Serial.println(temp_c);   
        Serial.println(humidity);
    }
}

void getSgp30Reading(){
  
  sgp.IAQmeasure();
  mVoc = sgp.TVOC;
  mCo2e = sgp.eCO2;
  
}

void sgp30SetBase(){
  sgp.getIAQBaseline(&eCo2_base, &Voc_base);
  EEPROMWrite(2, Voc_base);
  EEPROMWrite(0, eCo2_base);
}

void GetCO2PPM()
{
 Wire.beginTransmission(T6713_Address);
 for(int x=0; x<sizeof(cmdReadCo2); x++)
 {
    Wire.write(cmdReadCo2[x]);
  }
 // end transmission
 Wire.endTransmission();
 // read report of current gas measurement in ppm after delay!
 delay(10);
 Wire.requestFrom(T6713_Address, 4); // request 4 bytes from slave device
 data[0] = Wire.read();
 data[1] = Wire.read();
 data[2] = Wire.read();
 data[3] = Wire.read();
 CO2ppmValue = ((data[2] & 0x3F ) << 8) | data[3];
}


void EEPROMWrite(int address, uint16_t value){
//one is = MSB, two is LSB
  byte one = ((value>>8)& 0xFF);
  byte two = (value & 0xFF);

  EEPROM.write(address, two);
  EEPROM.write(address +1, one);
  EEPROM.commit();
  Serial.println("value saved");
}

uint16_t EEPROMRead(int address){
  //read 2 bytes from eeprom memory
  uint16_t two = EEPROM.read(address);
  uint16_t one = EEPROM.read(address+1);
  return ((two<<0)&0xFF)+ ((one<<8)&0xFFFF);
}
