 #define outputA 3
 #define outputB 4
 #define led 9
 int counter = 0; 
 int aState;
 int aLastState;  
 void setup() { 
   pinMode (outputA,INPUT);
   pinMode (outputB,INPUT);
   pinMode (led, OUTPUT);
   Serial.begin (9600);
   aLastState = digitalRead(outputA);   
 } 
 void loop() { 

  getPosition();
  analogWrite(led, counter); 
 }

 void getPosition(){
  
    aState = digitalRead(outputA); 
   if (aState != aLastState){     
    
     if (digitalRead(outputB) != aState) { 
     if(counter != 255){
        counter+=5;
       }
     } else {
       counter -=5;
       if(counter <0){
          counter =0;
        }
     }
     Serial.print("Position: ");
     Serial.println(counter);
   } 
   aLastState = aState; 
 }

