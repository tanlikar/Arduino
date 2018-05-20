#include <MQ2Lib.h>

int pin = A9; //change this to the pin that you use
float lpg = 0, co = 0, smoke = 0;

MQ2 mq2(pin, true); //instance (true=with serial output enabled)

void setup()
{
  Serial.begin(9600);
  mq2.begin();
}

void loop()
{
  /* Read all values from the sensor - it returns an array which contains 3 values
  * 1 = LPG in ppm
  * 2 = CO in ppm
  * 3 = SMOKE in ppm
  */
  float* values= mq2.read(true); //set it false if you don't want to print the values in the Serial

  //Reading specific values:
  //lpg = values[0];
  lpg = mq2.readLPG();
  //co = values[1];
  co = mq2.readCO();
  //smoke = values[2];
  smoke = mq2.readSmoke();

  //do something...

  delay(2000); //but values are read according to the rate defined in the library
}
