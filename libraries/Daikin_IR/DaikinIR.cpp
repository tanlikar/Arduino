#include "Arduino.h"
#include <DaikinIR.h>
#include <IRremote.h>
#include <IRremoteInt.h>

IRsend irsend;

daikinIr::daikinIr() {
	init();
}

void daikinIr::init(){
	irSignal[0] = HEADER1;
	irSignal[1] = HEADER2;
	irSignal[2] = MODE_1;
	irSignal[3] = FAN_SPEED_LOW;
	irSignal[4] = CLOCK_MINUTE_0;
	irSignal[5] = CLOCK_MINUTE_1;
	irSignal[6] = CLOCK_HOUR_0;
	irSignal[7] = CLOCK_HOUR_1;
	irSignal[8] = TIMER_MIN;
	irSignal[9] = TIMER_HR;
	irSignal[10] = TIMER_MIN;
	irSignal[11] = TIMER_HR;
	irSignal[12] = TEMP_0;
	irSignal[13] = TEMP_1;
	irSignal[14] = INIT_14;
	checksum();

}

String daikinIr::convert(uint8_t input ){
	switch (input){
		case 0:{
			return ("0000");
		}
		case 1:{
			return ("0001");
		}
		case 2:{
			return ("0010");
		}
		case 3:{
			return ("0011");
		}
		case 4:{
			return ("0100");
		}
		case 5:{
			return ("0101");
		}
		case 6:{
			return ("0110");
		}
		case 7:{
			return ("0111");
		}
		case 8:{
			return ("1000");
		}
		case 9:{
			return ("1001");
		}
		case 10:{
			return ("1010");
		}
		case 11:{
			return ("1011");
		}
		case 12:{
			return ("1100");
		}
		case 13:{
			return ("1101");
			break;
		}
		case 14:{
			return ("1110");
		}
		case 15:{
			return ("1111");
		}
		default : break;
	}
}

uint8_t daikinIr::rev(uint8_t b) {
   b = (b & 0b1100) >> 2 | (b & 0b0011) << 2;
   b = (b & 0b1010) >> 1 | (b & 0b0101) << 1;
   return b;
}

void daikinIr::checksum() {
  irSignal[15] = calcChecksum(irSignal);
}

uint8_t daikinIr::calcChecksum(uint8_t input[]) {

  uint8_t total=0;

  for (uint8_t i=0; i<15; i++) {
    total += (uint8_t)input[i];
  }

  return (total & (uint8_t)0xF);
}

void daikinIr::setSwingAuto(bool state){
	if(state)
		irSignal[14] = irSignal[14] | SWING_ON;
	else
		irSignal[14] = irSignal[14] & ~SWING_ON;
}

void daikinIr::setPower(bool state){
	if(state)
		irSignal[14] = irSignal[14] | POWER_ON;
	else
		irSignal[14] = irSignal[14] & ~POWER_ON;
}

void daikinIr::setSleep(bool state){
	if(state)
		irSignal[14] = irSignal[14] | SLEEP;
	else 
		irSignal[14] = irSignal[14] & ~SLEEP;
}

void daikinIr::setFan(FAN input){
	switch(input){
		case MIN:{
			irSignal[3] = FAN_SPEED_LOW;
			break;
		}
		case MEDIUM:{
			irSignal[3] = FAN_SPEED_MEDIUM;
			break;
		}
		case MAX:{
			irSignal[3] = FAN_SPEED_HIGH;
			break;
		}
		case TURBO:{
			irSignal[3] = FAN_TURBO;
			break;
		}
		case QUIET:{
			irSignal[3] = FAN_QUIET;
			break;
		}
		case AUTO:{
			irSignal[3] = FAN_AUTO;
			break;
		}

	}
}

void daikinIr::setTemp(int input){

	int temp3 = input;

	if(temp3 <16){
		temp3 = 16;
	}else if (temp3 >30){
		temp3 = 30;
	}

	int temp1 = temp3%10;
	int temp2 = temp3/10;

	irSignal[13] = temp2;
	irSignal[12] = temp1;
}


void daikinIr::setOnTime(int time, bool state){

}

void daikinIr::sendComm(){

	uint8_t revIrSignal[SIGNAL_SIZE];
	String payload = "";
	checksum();

	for(int x = 0; x<16 ; x++){
		revIrSignal[x] = rev(irSignal[x]);
		payload += convert(revIrSignal[x]);
	}

	irsend.enableIROut(freq);
  	for(int x = 0; x<2; x++){
    	irsend.mark(irHdrHigh);
    	irsend.space(irHdrLow);
  	}

  	irsend.mark(irIntroHigh);
  	irsend.space(irIntroLow);

  	for(int x = 0; x<payload.length(); x++)
 	{
    	irsend.mark(irspace);
    	irsend.space(payload[x] == '1' ? irOne : irZero);
    }

    irsend.mark(irspace);
    irsend.space(irSeparator);
    irsend.mark(irEnd);
    irsend.space(0);

}





