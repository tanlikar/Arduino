#define R0 5.7 

void setup() {
 Serial.begin(9600);
}
void loop() {
 float sensor_volt;
 float RS_gas; // Get value of RS in a GAS
 float ratio; // Get ratio RS_GAS/RS_air
 int sensorValue = analogRead(A0);
 sensor_volt=(float)sensorValue/1024*5.0;
 RS_gas = (5.0-sensor_volt)/sensor_volt; // omit *RL
 /*-Replace the name "R0" with the value of R0 in the demo of First Test -*/
 ratio = RS_gas/R0; // ratio = RS/R0
 /*-----------------------------------------------------------------------*/
 Serial.print("sensor_volt = ");
 Serial.println(sensor_volt);
 Serial.print("RS_ratio = ");
 Serial.println(RS_gas);
 Serial.print("Rs/R0 = ");
 Serial.println(ratio);
 Serial.print("\n\n");
 delay(1000);
}
