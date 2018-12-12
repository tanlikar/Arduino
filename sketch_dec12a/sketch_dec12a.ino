#include <hpma115s0.h>

hpma115s0 device = hpma115s0();

void setup() {
  // put your setup code here, to run once:
 if(!device.init()){
  Serial.println("init false");
 }
}

void loop() {
  // put your main code here, to run repeatedly:
  device.readPM();
  Serial.print("PM2.5: "); 
  Serial.println(device.getPM(PM25_TYPE));
  Serial.print("PM10: "); 
  Serial.println(device.getPM(PM10_TYPE));
  delay(1000);
}
