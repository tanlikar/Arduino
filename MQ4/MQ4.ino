#include "MQ4.h"


#define PIN_MQ4 A0
MQ4 mq4_sensor = MQ4(PIN_MQ4, true);


float resistance ;
float ppm ;
float voltage;

void setup() {
  Serial.begin(9600);
  mq4_sensor.begin();
}

void loop() {

  ppm = mq4_sensor.getPPM();
  voltage = mq4_sensor.getVoltage();
 
  Serial.print("\t PPM: ");
  Serial.println(ppm);
  Serial.print("\t voltage: ");
  Serial.println(voltage);

  delay(300);
}
