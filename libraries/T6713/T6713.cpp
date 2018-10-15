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

  Wire.beginTransmission(T6713_ADDRESS);

  for(int x=0; x<5 ; x++){

  	Wire.write(ReadCO2[x]);
  }
  // end transmission
  Wire.endTransmission();
  // read report of current gas measurement in ppm
  delay(2000); // can be shorter, need to test

  Wire.requestFrom(T6713_ADDRESS, 4);    // request 4 bytes from slave device

  for(int x; x<4 ; x++){
  	data[x]=Wire.read();
  }

  return ((data[2] * 0xFF ) + data[3]);
}