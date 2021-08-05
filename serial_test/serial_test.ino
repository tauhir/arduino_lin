#include <AltSoftSerial.h>
// software serial #1: TX = digital pin 10, RX = digital pin 11
AltSoftSerial portOne;
int incomingByte[4];

void setup() {
  // initialize serial:
  Serial.begin(19200);
  // reserve 200 bytes for the inputString:
  portOne.begin(19200);
  
}


//0x85 0x11 0x255 0x255 checksum
void loop() {
  // print the string when a newline arrives:
  int counter = 0;
  if (portOne.read() == 85){//sync byte
    while ((portOne.available() > 0)&(counter<5)) {
      incomingByte[counter] = portOne.read();
      counter++;
    }
    Serial.println("I received: ");
    for(int i=0;i<5;i++){
        Serial.print(incomingByte[i], DEC);
        Serial.print(" ");
    }
    Serial.println("");
    Serial.println("done");
  }
  else {
    return;
  }
}