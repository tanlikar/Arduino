#include "hpma115s0.h"
//#include <SoftwareSerial.h>

int TX = D8;
int RX = D7;
//int TX = 8;
//int RX = 9;
SoftwareSerial pm25_ser(RX, TX);

hpma115s0::hpma115s0():
pm25_val(0),
pm10_val(0)
{

}

bool hpma115s0::init(){
	Serial.begin(9600);
	pm25_ser.begin(9600);
	if(!pm25_ser.isListening()){
		Serial.println("[PM25] not listening");
		pm25_ser.listen();
	}

	return disableAutoSend();
}

void hpma115s0::sendCmd(const int *cmd){
	
	for(int x = 0 ; x<4 ; x++){
		pm25_ser.write(cmd[x]);
	}

	delay(10);
}

bool hpma115s0::readRes(int l = 32){
	int i = 0;
	int buf[l];

	while(pm25_ser.available() >0 && i< l){
		buf[i] = pm25_ser.read();

		if(DEBUG){
			Serial.print("i: "); Serial.print(i);
			Serial.print(" buf[i]: "); Serial.println(buf[i], HEX);
		}

		if(i == 0 && !(buf[0] == 0x40 || buf[0] == 0xA5 || buf[0] == 0x96)){
			if(DEBUG){
				Serial.println("skipping byte");
			}
			continue;
		}else {
			i++;
		}


	//autosend
		if(buf[0] == 0x42 && buf[1] == 0x4d){
			if(DEBUG){Serial.println("autosend");}
			l=32;
		}

	//reading
		if(buf[0] == 0x40 && buf[2] == 0x4){
			if(DEBUG){Serial.println("reading");}
			l=8;
		}

	//pos ack
		if(buf[0] == 0xA5 && buf[1] == 0xA5){
			if(DEBUG){Serial.println("ACK");}
			return true;
		}

	//neg. ack
		if(buf[0] == 0x96 && buf[1] == 0x96){
			if(DEBUG){Serial.println("NACK");}
			return false;
		}
	}

	//check checksum in reading
	if(buf[2] == 0x04){

		int cs = buf[0] + buf[1] + buf[2];
		int c;

		for(c= 3; c < (2 + buf[1]); c++){
			cs += buf[c];
		}

		cs = (65536 - cs) % 256;

		//validate checksum	
		if(cs == buf[c]){

			pm25_val = buf[3]*256 + buf[4];
			pm10_val = buf[5]*256 + buf[6];
			return true;
		}else{
			Serial.println("checksum mismatch");
		}
	}else if (buf[3] == 0x1c){
		int cs = 0;
		int c;

		for(c = 0; c<= buf[3]; c++){

			cs += buf[c];
		}
		int checksum = buf[30] * 256 + buf[31];
		if(DEBUG){
			Serial.print("checksum: ");Serial.print(checksum, HEX);
			Serial.print("CS: "); Serial.println(cs, HEX);
		}

		if(cs == checksum){
			pm25_val = buf[6] * 256 + buf[7];
			pm10_val = buf[8] * 256 + buf[9];
			return true;
		}else{
			Serial.println("checksum mismatch");
		}
	}else{

	}

	return false;

}

bool hpma115s0::disableAutoSend(){

	sendCmd(AutoSendOff);
	if(readRes()){
		Serial.println("autosend disable");
		isAutoSend = false;
		return true;
	}

	return false;
}

bool hpma115s0::enableAutoSend(){

	sendCmd(AutoSendOn);
	if(readRes()){
		Serial.println("autosend enable");
		isAutoSend = true;
		return true;
	}

	return false;
}

bool hpma115s0::readPM(){

	pm25_val = NAN;
	pm10_val = NAN;

	sendCmd(ReadPm);
	if(readRes()){
		Serial.println("PM readed");
		return true;
	}

	return false;
}


uint16_t hpma115s0::getPM(pm25_sensor_type PM_TYPE=PM25_TYPE){

	if(PM_TYPE == PM25_TYPE){
		return pm25_val;
	}else if (PM_TYPE == PM10_TYPE){
		return pm10_val;
	}
}


void hpma115s0::debug(bool debug){
	DEBUG = debug;
}

