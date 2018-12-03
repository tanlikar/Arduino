#include <Wire.h>
#include "Adafruit_SGP30.h"
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <pm25.h>
#include <SoftwareSerial.h>
SoftwareSerial pm25_ser(D7, D8);


LiquidCrystal_I2C lcd(0x27, 16, 2);

Adafruit_SGP30 sgp;

#include "SparkFunHTU21D.h"

#define T6713_Address 0x15 //T6713 i2C Address
byte data[6];
int CO2ppmValue;
const int cmdReadCo2[] = {0x04, 0x13, 0x8B, 0x00, 0x01};

//Create an instance of the object
HTU21D myHumidity;

/* return absolute humidity [mg/m^3] with approximation formula
  @param temperature [掳C]
  @param humidity [%RH]
*/
uint32_t getAbsoluteHumidity(float temperature, float humidity) {
  // approximation formula from Sensirion SGP30 Driver Integration chapter 3.15
  const float absoluteHumidity = 216.7f * ((humidity / 100.0f) * 6.112f * exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
  const uint32_t absoluteHumidityScaled = static_cast<uint32_t>(1000.0f * absoluteHumidity); // [mg/m^3]
  return absoluteHumidityScaled;
}

void PM25_listen() {
  if (!pm25_ser.isListening()) {
    Serial.println(F("[PM25] >> Listening"));
    pm25_ser.listen();
  }
}


void EEPROMWrite(int address, uint16_t value) {
  //one is = MSB, two is LSB
  byte one = ((value >> 8) & 0xFF);
  byte two = (value & 0xFF);

  EEPROM.write(address, two);
  EEPROM.write(address + 1, one);
  EEPROM.commit();
  Serial.println("value saved");
}

uint16_t EEPROMRead(int address) {
  //read 2 bytes from eeprom memory
  uint16_t two = EEPROM.read(address);
  uint16_t one = EEPROM.read(address + 1);
  return ((two << 0) & 0xFF) + ((one << 8) & 0xFFFF);
}

void GetCO2PPM()
{
  Wire.beginTransmission(T6713_Address);
  for (int x = 0; x < sizeof(cmdReadCo2); x++)
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

void setup() {
  Serial.begin(9600);
  Serial.println("SGP30 test");
  myHumidity.begin();
  EEPROM.begin(256);

  if (! sgp.begin()) {
    Serial.println("Sensor not found :(");
    while (1);
  }

  sgp.IAQinit();
  Serial.print("Found SGP30 serial #");
  Serial.print(sgp.serialnumber[0], HEX);
  Serial.print(sgp.serialnumber[1], HEX);
  Serial.println(sgp.serialnumber[2], HEX);


  // If you have a baseline measurement from before you can assign it to start, to 'self-calibrate'
  //sgp.setIAQBaseline(37426, 37515);
  uint16_t initVOC = EEPROMRead(2);
  uint16_t initCo2e = EEPROMRead(0);
  sgp.setIAQBaseline(initCo2e, initVOC);  // Will vary for each sensor!
  // Serial.println("**** on start eCO2 "+ String(initCo2e)+ " TVOC " + String(initVOC));

  Wire.begin(D2, D1);
  lcd.begin();
  lcd.home();

  pm25_ser.begin(9600);

  // set uart for pm25, and uart for debug
  // set listen function if one or more SoftwareSerials are used
  PM25.init(&pm25_ser, &Serial, PM25_listen);

}

int counter = 0;
void loop() {


  float humd = myHumidity.readHumidity();
  float temp = myHumidity.readTemperature();
  
  Serial.print(" Temperature:");
  Serial.print(temp, 2);
  Serial.print("C");
  Serial.print(" Humidity:");
  Serial.print(humd, 2);
  Serial.print("%");

  // If you have a temperature / humidity sensor, you can set the absolute humidity to enable the humditiy compensation for the air quality signals
  //float temperature = 22.1; // [掳C]
  //float humidity = 45.2; // [%RH]
  sgp.setHumidity(getAbsoluteHumidity(temp, humd));

  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }
  Serial.print(" TVOC "); Serial.print(sgp.TVOC); Serial.print(" ppb\t");
  Serial.print("eCO2 "); Serial.print(sgp.eCO2); Serial.println(" ppm");

  counter++;
  if (counter == 1800) {
    counter = 0;

    uint16_t TVOC_base, eCO2_base;
    if (! sgp.getIAQBaseline(&eCO2_base, &TVOC_base)) {
      Serial.println("Failed to get baseline readings");
      return;
    }
    Serial.println("**** check eCO2" + String(EEPROMRead(0)) + " TVOC " + String(EEPROMRead(2)));
    Serial.print("****Baseline values: eCO2: 0x"); Serial.print(eCO2_base, HEX);
    Serial.print(" & TVOC: 0x"); Serial.println(TVOC_base, HEX);
    EEPROMWrite(2, TVOC_base);
    EEPROMWrite(0, eCO2_base);
  }


  uint16_t val = PM25.read();
  uint16_t mPM10 = PM25.read(PM10_TYPE);
  Serial.print(F("PM2.5 val: "));
  Serial.print(val);
  Serial.println(F("  ug/m3"));
  Serial.print(F("PM10 val: "));
  Serial.print(mPM10);
  Serial.println(F("  ug/m3"));

  GetCO2PPM();
  Serial.print("Co2: ");
  Serial.println(CO2ppmValue);
  Serial.println("**** check eCO2" + String(EEPROMRead(0)) + " TVOC " + String(EEPROMRead(2)));



  String msg1 = (String)temp + " " + (String)humd + " " + (String)sgp.TVOC;
  String msg2 =  (String)sgp.eCO2 + " " +  (String)val + " " + (String)mPM10 + " " + (String)CO2ppmValue;
  lcd.clear();

  lcd.print(msg1);
  lcd.setCursor(0, 1);
  lcd.print(msg2);
  delay(1000);
}
