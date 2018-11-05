#include "T6713.h"
T6713 t6713;

void setup() {
  // put your setup code here, to run once:
  t6713.initialize();
  t6713.isConnected();
  
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(t6713.read());
  delay(1000);
}
