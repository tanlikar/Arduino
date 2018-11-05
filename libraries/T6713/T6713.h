#pragma once

#if defined(ARDUINO) && ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h>

#define T6713_ADDRESS 0x15  //Unshifted 7-bit I2C address for the sensor
const int ReadCO2[5] = {0x04, 0x13, 0x8B, 0x00, 0x01};

class T6713 {

public:
	T6713();  

  //Public Functions
	void begin(TwoWire &wirePort = Wire); //If user doesn't specificy then Wire will be used
 	int readCO2();
 	void debug(bool input);

  //Public Variables

private:
  //Private Functions

  //Private Variables
	int data[4];
	TwoWire *_i2cPort; 
	bool debug;

};
