#include "DaikinIRESP8266.h"

DaikinIRESP8266 temp = DaikinIRESP8266(4);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  temp.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  temp.setPower(true);
  temp.setTemp(25);
  temp.setFan(MAX);
  temp.setSwingAuto(true);
  temp.sendComm();
  delay(3000);
}
