

#include "MQ5.h"
#include <math.h>
//getppm have error


MQ5::MQ5(uint8_t pin, bool doSerial) {
  _pin = pin;
  _serial = doSerial;
}


float MQ5::getResistance() {
  int val = analogRead(_pin);
  return ((1023./(float)val) - 1.)*RLOAD;
}


float MQ5::getPPM() {

  
  return PARA_A * pow((getResistance()/RZERO), -PARA_B);
}



void MQ5::MQCalibration()
{
  temp1 = 0;

  for (int i=0; i<MQ5_CALIBARAION_SAMPLE_TIMES; i++) //take multiple samples
  {
    temp1 += getResistance();
    delay(MQ5_CALIBRATION_SAMPLE_INTERVAL);
  }

  temp1 = temp1/MQ5_CALIBARAION_SAMPLE_TIMES;    //average Rs value in clean air

  RZERO = temp1 / RATIO_AIR;

}

void MQ5::begin()
{
  MQCalibration();
  if (_serial)
  {
    Serial.print(F("MQ5 Ro: "));
    Serial.print(RZERO);
    Serial.println(F(" kohm"));
  }
}