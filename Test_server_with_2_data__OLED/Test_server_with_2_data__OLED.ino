//-- Libraries Included --------------------------------------------------------------
  #include <ESP8266WiFi.h>
  #include <SPI.h>
  #include <Wire.h>
  #include <Adafruit_SSD1306.h>
  #include <Adafruit_GFX.h>
//------------------------------------------------------------------------------------
  // Define I/O Pins
  #define     LED0      2           // WIFI Module LED
  #define OLED_RESET D5
  Adafruit_SSD1306 display(OLED_RESET);
  
//------------------------------------------------------------------------------------
  // Authentication Variables
  char*       NZHssid;              // SERVER WIFI NAME
  char*       NZHpassword;          // SERVER PASSWORD``
//------------------------------------------------------------------------------------
  #define     MAXSC     2           // MAXIMUM NUMBER OF CLIENTS
  WiFiServer  NZHServer(9001);      // THE SERVER AND THE PORT NUMBER
  WiFiClient  NZHClient[MAXSC];     // THE SERVER CLIENTS
  float sensorReading, sensorReading2;
  float data1,data2;
  String Received;
  char DATA1[50];
  String datavalue1, datavalue2;
  String realreadString, realreadString2;
  
//====================================================================================

  void setup()
  {
    // Setting The Serial Port
    Serial.begin(9600);           // Computer Communication
    
    // Setting The Mode Of Pins
    pinMode(LED0, OUTPUT);          // WIFI OnBoard LED Light

    // Print Message Of I/O Setting Progress
    Serial.println();
    Serial.println("I/O Pins Modes Set .... Done");

    // Setting Up A Wifi Access Point
    SetWifi("TEST", "12345678");

    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
    display.clearDisplay(); //clearing the display
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.println("Waiting for Client..");
    display.display();
  }

//====================================================================================
  
  void loop()
  {
    
    IsClients();
    
  }

//====================================================================================
  
  void SetWifi(char* Name, char* Password)
  {
    // Stop Any Previous WIFI
    WiFi.disconnect();

    // Setting The Wifi Mode
    WiFi.mode(WIFI_AP_STA);
    Serial.println("WIFI Mode : AccessPoint Station");
    
    // Setting The Access Point
    NZHssid      = Name;
    NZHpassword  = Password;
    
    // Starting The Access Point
    WiFi.softAP(NZHssid, NZHpassword);
    Serial.println("WIFI < " + String(NZHssid) + " > ... Started");
    
    // Wait For Few Seconds
    delay(1000);
    
    // Getting Server IP
    IPAddress IP = WiFi.softAPIP();
    
    // Printing The Server IP Address
    Serial.print("AccessPoint IP : ");
    Serial.println(IP);

    // Starting Server
    NZHServer.begin();
    Serial.println("Server Started");
  }

  void IsClients()
  {
    if (NZHServer.hasClient())
    {
      for(int i = 0; i < MAXSC; i++)
      {
        //find free/disconnected spot
        if (!NZHClient[i] || !NZHClient[i].connected())
        {
          if(NZHClient[i]) NZHClient[i].stop();
          NZHClient[i] = NZHServer.available();
          Serial.print("New Client : "); Serial.print(String(i+1) + " - ");
          continue;
        }
      }
      WiFiClient NZHClient = NZHServer.available();
      NZHClient.stop();
    }

    //check clients for data -------------------------------------------------------
    
    for(int i = 0; i < MAXSC; i++)
    {
      if (NZHClient[i] && NZHClient[i].connected())
      {
        if(NZHClient[i].available())
        {
          // If Any Data Was Available We Read IT
          while(NZHClient[i].available()) 
          {
            // Read From Client
            Received = NZHClient[i].readStringUntil('\n');
            data_check();
            data1 = realreadString.toFloat();
            data2 = realreadString2.toFloat();
            sensorReading = (data1/1023)*5;
            sensorReading2 = (data2/1023)*5;
            Serial.println(realreadString);
            display.clearDisplay();
            display_1();
            display.display(); //refreshing the display again.
          }
        }
        else
        {
            // This LED Blinks If Clients Where Available
            digitalWrite(LED0, HIGH);
            delay(50);
            digitalWrite(LED0, LOW);
            delay(50);
        }
      }
    }
  }

  void display_1()
  {
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("Pyrano:");
  // delay(1000);

    display.drawLine(64,0,64,32,WHITE);
    display.drawLine(0,8,128,8,WHITE);

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(68, 0);
    display.println("Pyrhelio: ");
  
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0, 15);
    display.print(sensorReading);
    display.println("V");

    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(68, 15);
    display.print(sensorReading2);
    display.print("V");
  
    display.display();  // now that the display is build, display it...
  }

void data_check()
{
  Received.toCharArray(DATA1,50);
  realreadString = "";
  if (DATA1[0] == '#')
      {
        for (int x = 1; x<50; x++)
        {
          if(DATA1[x]== '~'){
              break;
            }
          else{
              realreadString += DATA1[x];
            }
        }
        Serial.println( realreadString);
        
      /*  while(1){
          datavalue1 = DATA1;
          if (datavalue1 == "~"){
            break;
          }
          else
          {
            realreadString += datavalue1;
            //Serial.println (realreadString);
          }*/
        }
     
}
