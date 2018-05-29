#ifndef MQ4_h
#define MQ4_h

#include <Arduino.h>

    const float m = -0.318;
    const float b = 1.133;

    const float VOLT_RESOLUTION = 5.0; // if 3.3v use 3.3
    const int ADC_RESOLUTION = 10; // for 10bit analog to digital converter.
    const float RLOAD = 10;
    const float RATIO_AIR = 4.4;

#define  MQ4_CALIBARAION_SAMPLE_TIMES  100
#define  MQ4_CALIBRATION_SAMPLE_INTERVAL  50

class MQ4
{
  public:

    MQ4(uint8_t pin, bool doSerial);
   
 	float getResistance();
    float getPPM();
    void begin();
    double getVoltage();

  private:

    uint8_t _pin;
  	bool _serial;
  	void MQCalibration();
  	float temp1;
	float R0 = 11.820;

};

#endif