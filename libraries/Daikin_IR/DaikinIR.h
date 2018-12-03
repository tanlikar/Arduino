#ifndef DaikinIR_h
#define DaikinIR_h

#if(ARDUINO >= 100)
 #include "Arduino.h"
#else
 #include "WProgram.h"
 #include "pins_arduino.h"
 #include "WConstants.h"
#endif

#include <IRremote.h>
#include <IRremoteInt.h>

const unsigned int freq = 38;
const unsigned int irOne = 990;
const unsigned int irZero = 400;
const unsigned int irspace= 310;
const unsigned int irHdrHigh = 9770;
const unsigned int irHdrLow = 9830;
const unsigned int irIntroHigh = 4550;
const unsigned int irIntroLow = 2550;
const unsigned int irSeparator = 20250;
const unsigned int irEnd = 4550;

#define FAN_SPEED_LOW 0b1000
#define FAN_SPEED_MEDIUM 0b0100
#define FAN_SPEED_HIGH 0b0010
#define FAN_TURBO 0b0011
#define FAN_QUIET 0b1001
#define FAN_AUTO 0b0001

#define MODE_1 0b0010
#define MODE_2 0b0001
#define MODE_3 0b0100

#define CLOCK_MINUTE_0 0b0000
#define CLOCK_MINUTE_1 0b0000

#define CLOCK_HOUR_0 0b0000
#define CLOCK_HOUR_1 0b0000

#define TIMER_MIN 0b0000
#define TIMER_HR 0b0000

#define TEMP_0 0b0101
#define TEMP_1 0b0010

#define MAXTEMP 30
#define MINTEMP 16

#define SWING_ON 0b0001
#define SLEEP 0b0010

#define POWER_ON 0b1000
#define HEADER1 0b0110
#define HEADER2 0b0001
#define INIT_14 0b0100


#define SIGNAL_SIZE 16

enum FAN{
	MIN = 0,
	MEDIUM,
	MAX,
	TURBO,
	QUIET,
	AUTO
};

class daikinIr {
	public:
		daikinIr();
		void setSwingAuto(bool state);
		void setPower(bool state);
		void setSleep(bool state);
		void setFan(FAN input);
		void setTemp(int input);
		void sendComm();
		void setOnTime(int time, bool state);

	private:

		
		uint8_t irSignal[SIGNAL_SIZE];
		uint8_t rev(uint8_t b);
		void checksum();
		uint8_t calcChecksum(uint8_t input[]);
		String convert(uint8_t input );
		void init();


		IRsend irsend;

};

#endif