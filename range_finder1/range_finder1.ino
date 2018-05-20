#include <dht.h>
#include <LiquidCrystal.h>
#include <Servo.h> 


#define trigPin 12 // assign pin 12 for trigPin
#define echoPin 13 // assign pin 13 for echoPin
#define DHT11_PIN 11 // assign pin 11 for DHT11_PIN
#define led 10 // assign pin 10 for led

LiquidCrystal lcd(9, 8, 7, 6, 5, 4); // liquidcrystal (rs,enable,d4,d5,d6,d7)

dht DHT;
bool card = false;
Servo myservo;

void setup() {
  
  Serial.begin(9600); // set baud rate to 9600 for serial commucation
  pinMode(trigPin, OUTPUT); // assign trigPin as output
  pinMode(echoPin, INPUT); // assign echoPin as output
  pinMode(led, OUTPUT); // assign led as output
  myservo.attach(3); // assign pin 3 for servo
  myservo.write(0); // initialize servo 
}

void loop() {
  
  double duration, distance;
  float speed;

  //send out ultrasonic wave
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  duration = pulseIn(echoPin, HIGH); // duration = duration of echopin is high
  speed = 331.4 + (0.606 * DHT.temperature) + (0.0124 * DHT.humidity);
  distance = (duration / 2) * (speed / 10000);

if(Serial.available()) // check if have receive data from serial input
{ 
  delay(100); // wait for all the data to be received

  while (Serial.available()) 
  {
    card = Serial.read(); // data received saved in card

  }
}

if(card == true && (distance <=13))
  {
    myservo.write(90); // if card present and distance is less than 13 cm, the servo rotate 90 degree indicating gate open
    }
  if(card == true && (distance >13))
  {
    myservo.write(0); // if c
    card = false;
    }
    
  if (distance > 300)
  {
    //if range more than to 300 cm, lcd display "out of range" 
     lcd.print("Out of range");
     digitalWrite(led, HIGH); // set led to high indicating error
  }
   else {

   // lcd display distance when range is between 2 cm and 250 cm
    lcd.print("Dist : ");
    lcd.print(distance);
    lcd.print(" cm");
    delay(500);
   
  }
  

  delay(500);
  lcd.clear(); //clear lcd screen
   digitalWrite(led, LOW); 
}


