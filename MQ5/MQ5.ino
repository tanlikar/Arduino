#include "MQ5.h"


#define PIN_MQ5 A0
MQ5 mq5_sensor = MQ5(PIN_MQ5, true);

float correctedRZero;
float resistance ;
float ppm ;

void setup() {
  Serial.begin(9600);
  mq5_sensor.begin();
}

void loop() {

  ppm = mq5_sensor.getPPM();
 
  Serial.print("\t PPM: ");
  Serial.println(ppm);

  delay(300);
}
