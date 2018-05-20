#include <dht.h>

#define trigPin 12
#define echoPin 13
#define DHT11_PIN 11
#define ledred 9
 #define ledgreen 8
 
dht DHT;

void setup() {
  Serial.begin (9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(ledred, OUTPUT);
  pinMode(ledgreen, OUTPUT);
}

void loop() {
  
  float duration, distance;
  float speed;
  
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH);
  speed = 331.4 + (0.606 * DHT.temperature) + (0.0124 * DHT.humidity);
  distance = (duration / 2) * (speed / 10000);
  
  if (distance >= 10 || distance <= 2){
    digitalWrite(ledgreen, LOW);
  }
  else {
   digitalWrite(ledgreen, HIGH);
  }
 
}

