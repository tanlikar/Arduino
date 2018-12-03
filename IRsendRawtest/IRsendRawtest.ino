/*
 * IRremote: IRsendRawDemo - demonstrates sending IR codes with sendRaw
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 *
 * IRsendRawDemo - added by AnalysIR (via www.AnalysIR.com), 24 August 2015
 *
 * This example shows how to send a RAW signal using the IRremote library.
 * The example signal is actually a 32 bit NEC signal.
 * Remote Control button: LGTV Power On/Off. 
 * Hex Value: 0x20DF10EF, 32 bits
 * 
 * It is more efficient to use the sendNEC function to send NEC signals. 
 * Use of sendRaw here, serves only as an example of using the function.
 * 
 */


#include <IRremote.h>

IRsend irsend;

void setup()
{

}

void loop() {
  int khz = 38; // 38kHz carrier frequency for the NEC protocol
  unsigned int irSignal[] = {9694,     9853 ,    9728,     9875   ,  4502   ,  2583,
      275    ,  455   ,   275     ,1032   ,   275  ,   1032,
      275  ,    455   ,   274 ,    1034   ,   273 ,     455,
      277   ,   452   ,   274  ,    456   ,   274  ,    455,
      275 ,    1033  ,    275  ,    454    ,  274  ,    456,
      275  ,    455   ,   274  ,    455   ,   275   ,   454,
      274   ,  1033    ,  274   ,  1033    ,  275  ,   1032,
      275   ,   455   ,   274   ,   454    ,  276  ,    456,
      273  ,   1033    ,  275   ,   454   ,   274  ,    455,
      276   ,  1032    ,  274  ,    456  ,    274   ,   456,
      273   ,   455  ,    275   ,   456    ,  274   ,   454,
      275  ,    454   ,   275   ,   459    ,  270    ,  456,
      274   ,   455   ,   275   ,   455   ,   274   ,   456,
      273   ,   456   ,   274   ,   455    ,  275   ,   454,
      274  ,    456  ,    274   ,   455   ,   274 ,     456,
      274   ,   455   ,   275   ,   456  ,    272  ,    457,
      274  ,    456    ,  273   ,   455   ,   273  ,    456,
      275  ,   1033    ,  274   ,   455   ,   275  ,   1033,
      274  ,    455   ,   274    ,  456  ,    274   ,  1033,
      274  ,    454    ,  275     , 455 ,     278   ,   452,
      274 ,     456    ,  273    , 1033 ,     275  ,   1032,
      275  ,    455    ,  275  ,  1032  ,    275   ,   454,
      274  ,   1034    ,  274 ,   20329,
     4528};
     
  irsend.sendRaw(irSignal, sizeof(irSignal) / sizeof(irSignal[0]), khz); //Note the approach used to automatically calculate the size of the array.

  delay(5000); //In this example, the signal will be repeated every 5 seconds, approximately.
}
