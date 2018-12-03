#include <IRremote.h>
#include <IRremoteInt.h>

#define FAN_SPEED_LOW 0b1000
#define FAN_SPEED_MEDIUM 0b0100

const int freq = 38;
const int irOne = 990;
const int irZero = 400;
const int irspace= 310;
const int irHdrHigh = 9770;
const int irHdrLow = 9830;
const int irIntroHigh = 4550;
const int irIntroLow = 2550;
const int irSeparator = 20250;
const int irEnd = 4550;

int irSignal[] = {0,1,1,0,1,0,0,0,0,1,0,0,0,0,0,1,1,0,1,0,1,0,1,0,0,1,1,0,1,0,0,0,0,0,1,0,1,0,1,0,0,0,0,0,0,1,1,0,1,0,1,0,0,1,0,0,0,0,1,1,0,0,1,0};


IRsend irsend;

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
  irsend.enableIROut(freq);
  for(int x = 0; x<2; x++){
    irsend.mark(irHdrHigh);
    irsend.space(irHdrLow);
  }

  irsend.mark(irIntroHigh);
  irsend.space(irIntroLow);

  for(int x = 0; x<(sizeof(irSignal) / sizeof(irSignal[0])); x++)
  {
    irsend.mark(irspace);
     irsend.space(irSignal[x] == 1 ? irOne : irZero);
    }

    irsend.mark(irspace);
    irsend.space(irSeparator);
    irsend.mark(irEnd);
    irsend.space(0);

  delay(5000);
}
