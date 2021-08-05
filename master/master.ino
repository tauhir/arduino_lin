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
  	// 	LINBusSerialOne.begin(bound_rate); // config Serial
		// LINBusSerialOne.write(0x55); // write Synch Byte to serial
		// LINBusSerialOne.write(ident); // write Identification Byte to serial
		// for(int i=0;i<data_size;i++) LINBusSerialOne.write(data[i]); // write data to serial
		// LINBusSerialOne.write(checksum); // write Checksum Byte to serial
		// LINBusSerialOne.end(); // clear Serial config
}