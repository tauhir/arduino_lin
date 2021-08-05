#include <SoftwareSerial.h>
// software serial #1: TX = digital pin 10, RX = digital pin 11
SoftwareSerial portOne(10,11);
int incomingByte[4];

void setup() {
  // initialize serial:
  Serial.begin(19200);
  // reserve 200 bytes for the inputString:
  portOne.begin(19200);
  
}

// void loop() {
//   // print the string when a newline arrives:
//   int counter = 0;
//     while ((portOne.available() > 0)&(counter<6)) {
//         incomingByte[counter] = portOne.read();
//         counter++;
//     }
//     if (incomingByte[0]==0 & incomingByte[1] == 0){
//       return; //empty array check
//     }
//     else {
//           Serial.println("I received: ");
//     for(int i=0;i<5;i++){
//         Serial.print(incomingByte[i], DEC);
//         Serial.print(" ");
//     }
//     Serial.println("");
//     Serial.println("done");
//     }
    
// }


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