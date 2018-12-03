#include "DaikinIR.h"

daikinIr temp = daikinIr();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  temp.setPower(true);
  temp.setTemp(26);
  temp.setFan(MAX);
  temp.setSwingAuto(true);
  temp.sendComm();
  delay(3000);
}
