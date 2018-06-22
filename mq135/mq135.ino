#include "MQ135.h"

// MQ135 gas sensor
//
// Datasheet can be found here: https://www.olimex.com/Products/Components/Sensors/SNS-MQ135/resources/SNS-MQ135.pdf
//
// Application
// They are used in air quality control equipments for buildings/offices, are suitable for detecting of NH3, NOx, alcohol, Benzene, smoke, CO2, etc
//
// Original creator of this library: https://github.com/GeorgK/MQ135


#define PIN_MQ135 A0
MQ135 mq135_sensor = MQ135(PIN_MQ135, true);

float temperature = 21.0; // assume current temperature. Recommended to measure with DHT22
float humidity = 25.0; // assume current humidity. Recommended to measure with DHT22
float rzero ;
float correctedRZero;
float resistance ;
float ppm ;
float correctedPPM ;

void setup() {
  Serial.begin(115200);
  mq135_sensor.begin();
}

void loop() {

  mq135_sensor.MQCalibrationCorrection( temperature, humidity);
  ppm = mq135_sensor.getPPM();
  correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humidity);

  Serial.print("\t PPM: ");
  Serial.print(ppm);
  Serial.print("\t Corrected PPM: ");
  Serial.print(correctedPPM);
  Serial.println("ppm");

  delay(300);
}
