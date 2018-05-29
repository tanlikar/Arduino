#include "MQ4.h"


MQ4::MQ4(uint8_t pin, bool doSerial) {
  _pin = pin;
  _serial = doSerial;
}

  void MQ4::MQCalibration() {
     temp1 = 0;

  for (int i=0; i<MQ4_CALIBARAION_SAMPLE_TIMES; i++) //take multiple samples
  {
    temp1 += getResistance();
    delay(MQ4_CALIBRATION_SAMPLE_INTERVAL);
  }

    temp1 = temp1/MQ4_CALIBARAION_SAMPLE_TIMES;    //average Rs value in clean air

    R0 = temp1 / RATIO_AIR;
  }

  float MQ4::getResistance() {
    int val = analogRead(_pin);
    return ((1023./(float)val) - 1.)*RLOAD;
  }

    
    /**
     * Returns the PPM concentration
     */
    float MQ4::getPPM() {

      double RS_gas; //Define variable for sensor resistance
      double ratio; //Define variable for ratio
  
      RS_gas = getResistance();
  
      ratio = RS_gas / R0;   // Get ratio RS_gas/RS_air

      double ppm_log = (log10(ratio) - b) / m; //Get ppm value in linear scale according to the the ratio value
      double ppm = pow(10, ppm_log); //Convert ppm value to log scale
      return ppm;
  }

  void MQ4::begin()
{
  MQCalibration();
  if (_serial)
  {
    Serial.print(F("MQ4 Ro: "));
    Serial.print(R0);
    Serial.println(F(" kohm"));
  }
}

 double MQ4::getVoltage() {
    double val = 0.0;
    val = analogRead(_pin);
 
    double voltage = val * 5 / 1023;

    return voltage;
  }
