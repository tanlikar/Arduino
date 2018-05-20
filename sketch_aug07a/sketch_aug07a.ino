
#include <SPI.h>
#include <RFID.h>


RFID rfid(10, 9); 
byte card[5] = {208, 91, 65, 27, 209}; //set the desire UID
bool permission= false;

void setup()
{
  Serial.begin(9600); // set baud rate to 9600 for serial communucate
  SPI.begin(); //Initializes the SPI bus by setting SCK, MOSI, and SS to outputs, pulling SCK and MOSI low, and SS high.
  rfid.init();  //initialize rfid

}

void loop()

{
permission = false;

  if (rfid.isCard()) { // if card is present 
    
    rfid.readCardSerial(); // read the card UID

       for (int i = 0; i < 5; i++)
    {
      if (rfid.serNum[i] != card[i])  // compare the UID of the card with desire UID
      {
        permission = false;

      }
      else
      {
        permission = true;
       }
      
    }
    if (permission == true) 
    {
      Serial.write(permission); // send permission to the main arduino via serial communication

    }

    rfid.halt(); // stop reading card
  }
                         
}
