#include "Arduino.h"
#include <hpma115S0.h>

HPMA115S0 hpma115S0(Serial1);

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(19200);
  Serial1.begin(9600);
  delay(1000);
  Serial.println("Starting...");
  hpma115S0.Init();
  hpma115S0.StartParticleMeasurement();
}

// the loop function runs over and over again forever
void loop() {
  unsigned int pm2_5, pm10;
  if (hpma115S0.ReadParticleMeasurement(&pm2_5, &pm10)) {
  Serial.println("PM 2.5: " + String(pm2_5) + " ug/m3" );
  Serial.println("PM 10: " + String(pm10) + " ug/m3" );
  }
}
