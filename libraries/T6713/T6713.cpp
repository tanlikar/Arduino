#include "T6713.h"
#include <Wire.h>

T6713::T6713(){
}

void T6713::begin(TwoWire &wirePort)
{
  _i2cPort = &wirePort; //Grab which port the user wants us to use
  
  _i2cPort->begin();
}


int T6713::readCO2(){

 Wire.beginTransmission(ADDR_6713);
  Wire.write(0x04); Wire.write(0x13); Wire.write(0x8B); Wire.write(0x00); Wire.write(0x01);
  // end transmission
  Wire.endTransmission();
  // read report of current gas measurement in ppm
  delay(200);

  Wire.requestFrom(ADDR_6713, 4);    // request 4 bytes from slave device
  data[0] = Wire.read();
  data[1] = Wire.read();
  data[2] = Wire.read();
  data[3] = Wire.read();

	if(debug){
    	Serial.print("Func code: "); Serial.print(data[0],HEX);
    	Serial.print(" byte count: "); Serial.println(data[1],HEX);
    	Serial.print("MSB: 0x");  Serial.print(data[2],HEX); Serial.print("  ");
    	Serial.print("LSB: 0x");  Serial.print(data[3],HEX); Serial.print("  ");
      Serial.print("CO2ppm: "); Serial.print((data[2] * 0xFF ) + data[3]);
	}

   return ((data[2] * 0xFF ) + data[3]);
}

void debug(bool input){

	debug = input;
}