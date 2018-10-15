/*
 * Read particle matter using a Honeywell HPMA115S0-XXX and a ESP8266 (WeMos Mini)
 */
#include <SoftwareSerial.h>

#define DEBUG false
#define pin_rx D5
#define pin_tx D6

SoftwareSerial Device(pin_rx, pin_tx);

const int AutoSendOn[4] =     {0x68, 0x01, 0x40, 0x57};
const int AutoSendOff[4] =    {0x68, 0x01, 0x20, 0x77};
const int StartPmMeasure[4] = {0x68, 0x01, 0x01, 0x96};
const int StopPmMeasure[4] =  {0x68, 0x01, 0x02, 0x95};
const int ReadPm[4] =         {0x68, 0x01, 0x04, 0x93};

int isAutoSend = true;
int useReading = true;

int pm25 = 0;   // PM2.5
int pm10 = 0;   // PM10

unsigned long lastReading = 0;

void sendCommand(const int *cmd) {
  int i;
  for(i=0;i<4; i++) {
    Device.write(cmd[i]);
  }
  // let a unicorn pass
  delay(10);
}

int readResponse(int l = 32) {
  int i = 0;
  int buf[l];
  
  unsigned long start = millis();

  while(Device.available() > 0 && i < l) {

    buf[i] = Device.read();                 // read bytes from device

    if(DEBUG) {
      Serial.print("i: "); Serial.print(i);
      Serial.print(" buf[i]: "); Serial.println(buf[i], HEX);
    }

    // check for HEAD or skip a byte
    if(i == 0 && !(buf[0] == 0x40 || buf[0] == 0x42 || buf[0] == 0xA5 || buf[0] == 0x96)) {
      if(DEBUG) { Serial.println("Skipping Byte"); }
      continue;
    } else {
      i++;
    }

    if(buf[0] == 0x42 && buf[1] == 0x4d) {  // Autosend
      if(DEBUG) { Serial.println("Autosend"); }
      l=32;
    }

    if(buf[0] == 0x40 && buf[2] == 0x4) {   // Reading
      if(DEBUG) { Serial.println("Reading"); }
      l=8;
    }

    if(buf[0] == 0xA5 && buf[1] == 0xA5) {  // Pos. ACK
      if(DEBUG) { Serial.println("ACK"); }
      return true;
    }
    
    if(buf[0] == 0x96 && buf[1] == 0x96) {  // Neg. ACK
      if(DEBUG) { Serial.println("NACK"); }
      return false;
    }

    if (millis() - start > 1000) {          // trigger Timeout after 1 sec
      Serial.println("Timeout");
      return false;
    }

  }

  // check checksum in Reading
  if(buf[2] == 0x04) {
    // HEAD+LEN+CMD
    int cs = buf[0] + buf[1] + buf[2];
    int c;
    
    // DATA
    for(c = 3; c < (2 + buf[1]); c++) {
      // Serial.println(buf[c]);
      cs += buf[c];
    }
    // CS = MOD((65536-(HEAD+LEN+CMD+DATA)), 256)
    cs = (65536 - cs) % 256;
    
    // validate checksum
    if(cs == buf[c]) {
      // calculate PM values
      pm25 = buf[3] * 256 + buf[4];
      pm10 = buf[5] * 256 + buf[6];
      return true;
    } else {
      Serial.println("Checksum mismatch");
    }
  } else if(buf[3] == 0x1c) {  // Autoreading
    int cs = 0;
    int c;
    // DATA
    for(c = 0; c <= buf[3]; c++) {
      // Serial.println(buf[c]);
      cs += buf[c];
    }
    int checksum = buf[30] * 256 + buf[31];
    if(DEBUG) {
      Serial.print("Checksum: "); Serial.print(checksum, HEX);
      Serial.print(" CS: "); Serial.println(cs, HEX);
    }

    if(cs == checksum) {
      // calculate PM values
      pm25 = buf[6] * 256 + buf[7];
      pm10 = buf[8] * 256 + buf[9];
      return true;
    } else {
      Serial.println("Checksum mismatch");
    }
  } else {
    // unkown
  }
  
  return false;
}

void setup() {
  // init Serial for ESP8266
  Serial.begin(115200); Serial.println();
  // init Serial for Device
  Device.begin(9600); Device.println();
}

void loop() {

  if(millis() - lastReading >= 1000 || lastReading == 0) {
    lastReading = millis();

    // handle AutoSend
    if(isAutoSend) {
      if(useReading) {
        if(readResponse()) {
          Serial.print("PM 2.5: "); Serial.print(pm25);
          Serial.print(" / PM 10: "); Serial.println(pm10);
        }
      } else { // disable
        Serial.println("Stop AutoSend");
        sendCommand(AutoSendOff);
        if(readResponse()) {
          Serial.println("AutoSend disabled.");
          isAutoSend = !isAutoSend;
        }
      }
    } else {
      sendCommand(ReadPm);
      if(readResponse()) {
        Serial.print("PM 2.5: "); Serial.print(pm25);
        Serial.print(" / PM 10: "); Serial.println(pm10);
      }
    }
  }
}
