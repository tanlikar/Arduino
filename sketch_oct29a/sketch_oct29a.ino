#include <EEPROM.h>


void EEPROMWrite(int address, uint16_t value){
//one is = MSB, two is LSB
  byte one = ((value>>8)& 0xFF);
  byte two = (value & 0xFF);

  EEPROM.write(address, two);
  EEPROM.write(address +1, one);
  EEPROM.commit();
  Serial.println("value saved");
}

uint16_t EEPROMRead(int address){
  //read 2 bytes from eeprom memory
  uint16_t two = EEPROM.read(address);
  uint16_t one = EEPROM.read(address+1);
  return ((two<<0)&0xFF)+ ((one<<8)&0xFFFF);
}


void setup() {
  Serial.begin(9600);
}

void loop() {
  uint16_t initVOC = EEPROMRead(2);
 uint16_t initCo2e = EEPROMRead(0);
   Serial.println("**** on start eCO2 "+ String(initCo2e)+ " TVOC " + String(initVOC));

}


