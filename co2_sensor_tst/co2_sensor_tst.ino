// CO2 Meter Amphenol T6713 i2c Example Interface
// By Marv Kausch, 12/2016 at CO2 Meter <co2meter.com>
// Revised by John Houck, 05/01/2018
// Talks via I2C to T6713 sensors and displays CO2 values
// We will be using the I2C hardware interface on the Arduino in
// combination with the built-in Wire library to interface.
// Arduino analog input 5 - I2C SCL
// Arduino analog input 4 - I2C SDA
/*
 In this example we will do a basic read of the CO2 value and
checksum verification.
 For more advanced applications please see the I2C Comm guide.
*/
#include <Wire.h> // Although Wire.h is part of the Ardunio GUI
#define T6713_Address 0x15 //T6713 i2C Address

byte data[6];
int CO2ppmValue;
 
void setup(){
    Serial.begin(9600);
    Serial.println("14CORE | T67XX / AN161 CO2 Sensor Test Code");
    Serial.println("Initializing...............................");
    delay(2000);
    Serial.println("Starting i2C Communicate ADDR 0x15.........");
    Wire.begin();
    delay(1000);

    ABCalgo;
}
 
void loop(){
GetCO2PPM();
if (CO2ppmValue > 0) {
    Serial.println("CO2-Carbon Dioxide Read Value > ");
    Serial.println(CO2ppmValue);
} else {
  Serial.println("ERROR | Failed to communicate to the sensor");
}
    delay(2000);
}

void GetCO2PPM()
{
 // start I2

 Wire.beginTransmission(T6713_Address);
 Wire.write(0x04);
 Wire.write(0x13);
 Wire.write(0x8B);
 Wire.write(0x00);
 Wire.write(0x01);
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

void ABCalgo(){
  Wire.beginTransmission(T6713_Address);
  Wire.write(0x05);
  Wire.write(0x03);
  Wire.write(0xEE);
  Wire.write(0x00);
  Wire.write(0x00);
 // end transmission
  Wire.endTransmission();
}

