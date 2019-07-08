int sensor_pin = A0;
int output_value ;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Reading From the Sensor ...");
  delay(2000); 
}

void loop() {
  // put your main code here, to run repeatedly:
  output_value= analogRead(sensor_pin);
  Serial.print("Moisture : ");
  Serial.print(output_value);
  Serial.print("\n");delay(1000);
}
