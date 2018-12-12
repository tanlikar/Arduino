#ifndef hpma115s0_h
#define hpma115s0_h

 #if(ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
 #include "pins_arduino.h"
 #include "WConstants.h"
#endif

#include <SoftwareSerial.h>
#include <cmath>

typedef enum pm25_sensor_type {
	PM25_TYPE,
	PM10_TYPE
};

const int AutoSendOn[4] =     {0x68, 0x01, 0x40, 0x57};
const int AutoSendOff[4] =    {0x68, 0x01, 0x20, 0x77};
const int StartPmMeasure[4] = {0x68, 0x01, 0x01, 0x96};
const int StopPmMeasure[4] =  {0x68, 0x01, 0x02, 0x95};
const int ReadPm[4] =         {0x68, 0x01, 0x04, 0x93};

class hpma115s0 {

public:
	hpma115s0();
	void sendCmd(const int *cmd);
	bool readRes(int l);
	bool disableAutoSend();
	bool enableAutoSend();
	bool readPM();
	bool init();
	void debug(bool debug);
	uint16_t getPM(pm25_sensor_type PM_TYPE);

private:

	uint16_t pm25_val;
	uint16_t pm10_val;
	int isAutoSend = true;
	int useReading = true;
	int DEBUG = false;
};

#endif