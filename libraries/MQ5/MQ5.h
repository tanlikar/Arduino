
#ifndef MQ5_H
#define MQ5_H
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

/// The load resistance on the board
#define RLOAD 10.0 // can be omitted (rs/r0 will cancel out the RL)
#define RATIO_AIR 6.5 
#define PARA_A 75.711
#define PARA_B 2.542


#define  MQ5_CALIBARAION_SAMPLE_TIMES  5
#define  MQ5_CALIBRATION_SAMPLE_INTERVAL  50

class MQ5 {
 private:
  uint8_t _pin;
  bool _serial;
  void MQCalibration();
  float temp1; // Rs_air
  float RZERO;


 public:
  MQ5(uint8_t pin, bool doSerial);
  float getResistance();
  float getPPM();
  void begin();
    
};
#endif
