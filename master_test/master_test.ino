//USING lIBRARY - uncomment below:

#include "lin_stack.h"

// LIN Object
lin_stack LIN1(1);

void setup() {
  // Nothing to do here.
}
//0x85 0x11 0x255 0x255 checksum
void loop() {
  // Create Data Package
  byte packageON[] = {255, 255}; // LEDs ON
  LIN1.write(0x11, packageON, 2);
  delay(5000);
  byte packageOFF[] = {0, 0}; // LEDs OFF
  LIN1.write(0x11, packageOFF, 2);
  delay(5000);
}

//PURE TEST -- uncomment below
// #include <AltSoftSerial.h>
// AltSoftSerial LINBusTestOne;
// byte packageON[] = {255, 255};
// byte packageOFF[] = {0, 0}; 

// void setup() {
//   // Nothing to do here.
//   LINBusTestOne.begin(19200);
// }
// //0x85 0x11 0x255 0x255 checksum
// // dummy checksum 0x23

// void loop() {
//   LINBusTestOne.write(0x55); // write Synch Byte to serial
//   //should do microsend delay here
//   LINBusTestOne.write(0x11); // write Identification Byte to serial
//   for(int i=0;i<2;i++) LINBusTestOne.write(packageON[i]); // write data to serial
//   LINBusTestOne.write(0x23); // write Checksum Byte to serial
//   delay(5000);
  
//   LINBusTestOne.write(0x55); // write Synch Byte to serial
//   //should do microsend delay here
//   LINBusTestOne.write(0x11); // write Identification Byte to serial
//   for(int i=0;i<2;i++) LINBusTestOne.write(packageOFF[i]); // write data to serial
//   LINBusTestOne.write(0x23); // write Checksum Byte to serial
//   delay(5000);
// }