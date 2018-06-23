#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <LiquidCrystal_I2C.h>

#include <Wire.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);


// Update these with values suitable for your network.

const char* ssid = "tan";
const char* password = "123456789";
const char* mqtt_server = "192.168.137.139";
const char* clientID = "NodeMCUDevKit6";

//intopic esp8266 mqtt input
const char* inTopic = "textMQTT";


// Generally, you should use "unsigned long" for variables that hold time
unsigned long previousMillis = 0;        // will store last temp was read
const long interval = 1000;              // interval at which to read sensor
 
WiFiClient espClient;
PubSubClient client(espClient);
String msg1, msg2;

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

if(strcmp(topic, inTopic)==0){
    msg1 = message;
    lcd.clear();

   lcd.print(msg1);

    for(int x=0; x<length; x++)
    {  
      if(x>15){
      lcd.scrollDisplayLeft();
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
      client.subscribe(inTopic);
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
  Wire.begin(D2, D1);
  lcd.begin();
  lcd.home();
  lcd.leftToRight();
  lcd.cursor();
  lcd.blink();


}

void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
