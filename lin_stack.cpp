/*  LIN STACK for MCP2004
 *  v2.0
 *
 *  Short description: 
 *  Comunication stack for LIN and MCP2004 LIN transceiver. Built off of https://github.com/macchina/LIN/
 *  Currently for use with Arduino Mega 2560
 *  
 *  Author: Tauhir Edwards
 *  Arduino IDE 1.6.9
*/ 

/* LIN PACKET:
   It consist of:
    ___________ __________ _______ ____________ _________ 
   |           |          |       |            |         |
   |Synch Break|Synch Byte|ID byte| Data Bytes |Checksum |
   |___________|__________|_______|____________|_________|
   
   Every byte have start bit and stop bit and it is send LSB first.
   Synch Break - 13 bits of dominant state ("0"), followed by 1 bit recesive state ("1")
   Synch Byte - Byte for Bound rate syncronization, always 0x55
   ID Byte - consist of parity, length and address; parity is determined by LIN standard and depends from address and message length
   Data Bytes - user defined; depend on devices on LIN bus -at most this can be 8 bytes
   Checksum - inverted 256 checksum; data bytes are sumed up and then inverted
*/

// CONSTRUCTORS

#include "lin_stack.h"
SoftwareSerial LINBusSerialOne(10,11);
SoftwareSerial LINBusSerialTwo(12,13);

lin_stack::lin_stack(byte Ch){
	sleep_config(Ch); // Configurating Sleep pin for transceiver
	ch = Ch;
}

lin_stack::lin_stack(byte Ch, byte ident){
	sleep_config(Ch); // Configuration of Sleep pin for transceiver
	identByte = ident; // saving idet to private variable
	sleep(1); // Transceiver is always in Normal Mode
}

// PUBLIC METHODS
// WRITE methods
// Creates a LIN packet and then send it via USART(Serial) interface.
int lin_stack::write(byte ident, byte data[], byte data_size){
	// Calculate checksum
	byte suma = 0;
	for(int i=0;i<data_size;i++) 
		suma = suma + data[i];
	//suma = suma + 1;
	byte checksum = 255 - suma;
	// Start interface
	sleep(1); // Go to Normal mode
	// Synch Break
	serial_pause(13);
	// Send data via Serial interface
	if(ch==1){ // For LIN1 or LINBusSerialOne
		LINBusSerialOne.begin(bound_rate); // config Serial
		LINBusSerialOne.write(0x55); // write Synch Byte to serial
		LINBusSerialOne.write(ident); // write Identification Byte to serial
		for(int i=0;i<data_size;i++) LINBusSerialOne.write(data[i]); // write data to serial
		LINBusSerialOne.write(checksum); // write Checksum Byte to serial
		LINBusSerialOne.end(); // clear Serial config
	}else if(ch==2){ // For LIN2 or LINBusSerialTwo
		LINBusSerialTwo.begin(bound_rate); // config Serial
		LINBusSerialTwo.write(0x55); // write Synch Byte to serial
		LINBusSerialTwo.write(ident); // write Identification Byte to serialv
		for(int i=0;i<data_size;i++) LINBusSerialTwo.write(data[i]); // write data to serial
		LINBusSerialTwo.write(checksum);// write Checksum Byte to serial
		LINBusSerialTwo.end(); // clear Serial config
	}
	sleep(0); // Go to Sleep mode
	return 1;
}

int lin_stack::writeRequest(byte ident){
	// Create Header
	byte identByte = (ident&0x3f) | calcIdentParity(ident);
	byte header[2]= {0x55, identByte};
	// Start interface
	sleep(1); // Go to Normal mode
	// Synch Break
	serial_pause(13);
	// Send data via Serial interface
	if(ch==1){ // For LIN1 or LINBusSerialOne
		LINBusSerialOne.begin(bound_rate); // config Serial
		LINBusSerialOne.write(header,2); // write data to serial
		LINBusSerialOne.end(); // clear Serial config
	}else if(ch==2){ // For LIN2 or LINBusSerialTwo
		LINBusSerialTwo.begin(bound_rate); // config Serial
		LINBusSerialTwo.write(header,2); // write data to serial
		LINBusSerialTwo.end(); // clear Serial config
	}
	sleep(0); // Go to Sleep mode
	return 1;
}

int lin_stack::writeResponse(byte data[], byte data_size){
	// Calculate checksum
	byte suma = 0;
	for(int i=0;i<data_size;i++) suma = suma + data[i];
	//suma = suma + 1;
	byte checksum = 255 - suma;
	// Start interface
	sleep(1); // Go to Normal mode
	// Send data via Serial interface
	if(ch==1){ // For LIN1 or LINBusSerialOne
		LINBusSerialOne.begin(bound_rate); // config Serial
		LINBusSerialOne.write(data, data_size); // write data to serial
		LINBusSerialOne.write(checksum); // write data to serial
		LINBusSerialOne.end(); // clear Serial config
	}else if(ch==2){ // For LIN2 or LINBusSerialTwo
		LINBusSerialTwo.begin(bound_rate); // config Serial
		LINBusSerialTwo.write(data, data_size); // write data to serial
		LINBusSerialTwo.write(checksum); // write data to serial
		LINBusSerialTwo.end(); // clear Serial config
	}
	sleep(0); // Go to Sleep mode
	return 1;
}

int lin_stack::writeStream(byte data[], byte data_size){
	// Start interface
	sleep(1); // Go to Normal mode
	// Synch Break
	serial_pause(13);
	// Send data via Serial interface
	if(ch==1){ // For LIN1 or LINBusSerialOne
		LINBusSerialOne.begin(bound_rate); // config Serial
		for(int i=0;i<data_size;i++) LINBusSerialOne.write(data[i]);
		LINBusSerialOne.end(); // clear Serial config
	}else if(ch==2){ // For LIN2 or LINBusSerialTwo
		LINBusSerialTwo.begin(bound_rate); // config Serial
		for(int i=0;i<data_size;i++) LINBusSerialTwo.write(data[i]);
		LINBusSerialTwo.end(); // clear Serial config
	}
	sleep(0); // Go to Sleep mode
	return 1;
}

// READ methods
// Read LIN traffic and then proces it.
int lin_stack::setSerial(){ // Only needed when receiving signals
	if(ch==1){ // For LIN1 (Channel 1)
		LINBusSerialOne.begin(bound_rate); // Configure LINBusSerialOne
	} else if(ch==2){ // For LIN2 (Channel 2)
		LINBusSerialTwo.begin(bound_rate); // Configure LINBusSerialTwo
	}
}

int lin_stack::read(byte data[], byte data_size, boolean all_data = false, boolean id_specific = true){
	// added all_data to instead of just providing data, send back all values (sync, id, checksum etc)
	// id_specific when false will read everything and not check parity.
	byte rec[data_size+3]; //byte array of data+sync+id+checksum
	if(ch==1){ // For LIN1 or LINBusSerialOne
		if(LINBusSerialOne.read() != -1){ // Check if there is an event on LIN bus
			LINBusSerialOne.readBytes(rec,data_size+3);
		}
	}
	else if(ch==2){ // For LIN2 or LINBusSerialTwo
		if(LINBusSerialTwo.read() != -1){ // Check if there is an event on LIN bus
			LINBusSerialTwo.readBytes(rec,data_size+3);
		}
	}
	if((id_specific==true)&(validateParity(rec[1]))&(validateChecksum(rec,data_size+3))){

		if (all_data) {
			data = rec;
		}
		else {
			for(int j=0;j<data_size;j++){
				data[j] = rec[j+2];
			}
		}
		return 1;
	}
	else if(rec[0] == rec[-1]){
		return -1;
	}
	else if((id_specific == false) && (rec[0] != rec[-1])) {
		if (all_data) {
			data = rec;
		}
		else {
			for(int j=0;j<data_size;j++){
				data[j] = rec[j+2];
			}
		}
		if (data[0]== data[-1]){
			return 0;
		}
		return 1;
	}
	else{
		return -1;
	}	
	return 0;
}

// int lin_stack::readStream(byte data[],byte data_size){
// 	// how was this different to read?
// 	byte rec[data_size];
// 	if(ch==1){ // For LIN1 or LINBusSerialOne
// 		if(LINBusSerialOne.read() != -1){ // Check if there is an event on LIN bus
// 			LINBusSerialOne.readBytes(rec,data_size);
// 			for(int j=0;j<data_size;j++){
// 				data[j] = rec[j];
// 			}
// 			return 1;
// 		}
// 	}else if(ch==2){ // For LIN2 or LINBusSerialTwo
// 		if(LINBusSerialTwo.read() != -1){ // Check if there is an event on LIN bus
// 			LINBusSerialTwo.readBytes(data,data_size);
// 			return 1;
// 		}
// 	}
// 	return 0;
// }


// PRIVATE METHODS
// int lin_stack::serial_pause(int no_bits){
// 	// Calculate delay needed for 13 bits, depends on bound rate
// 	unsigned int del = period*no_bits; // delay for number of bits (no-bits) in microseconds, depends on period
// 	if(ch==2){
// 		PIOA->PIO_PER = PIO_PA13; // enable PIO register
// 		PIOA->PIO_OER = PIO_PA13; // enable PA13 as output
// 		PIOA->PIO_CODR = PIO_PA13; // clear PA13
// 		delayMicroseconds(del); // delay
// 		PIOA->PIO_SODR = PIO_PA13; // set pin high
// 		PIOA->PIO_ODR = PIO_PA13; // enable PA13 as output
// 		PIOA->PIO_PDR = PIO_PA13; // clear configuration for PIO, needs to be done because Serial wont work with it
// 	}else if(ch==1){
// 		PIOA->PIO_PER = PIO_PA11; // enable PIO register
// 		PIOA->PIO_OER = PIO_PA11; // enable PA11 as output
// 		PIOA->PIO_CODR = PIO_PA11; // clear PA11
// 		delayMicroseconds(del); // delay
// 		PIOA->PIO_SODR = PIO_PA11; // set pin high
// 		PIOA->PIO_ODR = PIO_PA11; // enable PA13 as output
// 		PIOA->PIO_PDR = PIO_PA11; // clear configuration for PIO, needs to be done because Serial wont work with it
// 	}
// 	return 1;
// }

// int lin_stack::sleep(byte sleep_state){
// 	if(sleep_state==1){ // Go to Normal mode
// 		if(ch==1) PIOB->PIO_SODR = PIO_PB4; // Set PB4, high state, normal mode
// 		if(ch==2) PIOB->PIO_SODR = PIO_PB7; // Set PB7, high state, normal mode
// 	}else if(sleep_state==0){ // Go to Sleep mode
// 		if(ch==1) PIOB->PIO_CODR = PIO_PB4; // Clear PB4, low state, sleep mode
// 		if(ch==2) PIOB->PIO_CODR = PIO_PB7; // Clear PB7, low state, sleep mode
// 	}
// 	delayMicroseconds(20); // According to TJA1021 datasheet this is needed for proper working
// 	return 1;
// }

int lin_stack::sleep_config(byte serial_No){
	if(serial_No==1){ // When using LIN1 channel - usign LINBusSerialOne and pin PB4 for Sleep
		pinMode(9, OUTPUT);
		digitalWrite(9, HIGH);
		pinMode(8, OUTPUT);
		digitalWrite(8, HIGH);
		ch=1; // saved as private variable, used for determening Serial port
	}else if(serial_No==2)
	{ // When using LIN2 channel - usign LINBusSerialTwo and pin PB7 for Sleep
		pinMode(7, OUTPUT);
		digitalWrite(7, HIGH);
		pinMode(6, OUTPUT);
		digitalWrite(6, HIGH);
		ch=2; // saved as private variable, used for determening Serial port
	}
	return 1;
}

boolean lin_stack::validateParity(byte ident) {
	if(ident == identByte)
		return true;
	else
		return false;
}

boolean lin_stack::validateChecksum(unsigned char data[], byte data_size){
	byte checksum = data[data_size-1];
	byte suma = 0;
	for(int i=2;i<data_size-1;i++) 
		suma = suma + data[i];
	byte v_checksum = 255 - suma - 1;
	if(checksum==v_checksum)
		return true;
	else
		return false;
} 

// int lin_stack::busWakeUp()
// {
//   unsigned int del = period*10; // random delay for dominant signal, has to be in the timeframe from 250us ... 5ms
//   if(ch==2)
//   {
//     PIOA->PIO_PER = PIO_PA13; // enable PIO register
//     PIOA->PIO_OER = PIO_PA13; // enable PA13 as output
//     PIOA->PIO_CODR = PIO_PA13; // clear PA13
//     delayMicroseconds(del); // delay
//     PIOA->PIO_SODR = PIO_PA13; // set pin high
//     PIOA->PIO_PDR = PIO_PA13; // clear configuration for PIO, needs to be done because Serial wont work with it
//   }
//   else if(ch==1)
//   {
//     PIOA->PIO_PER = PIO_PA11; // enable PIO register
//     PIOA->PIO_OER = PIO_PA11; // enable PA11 as output
//     PIOA->PIO_CODR = PIO_PA11; // clear PA11
//     delayMicroseconds(del); // delay
//     PIOA->PIO_SODR = PIO_PA11; // set pin high
//     PIOA->PIO_PDR = PIO_PA11; // clear configuration for PIO, needs to be done because Serial wont work with it
//   }
//   return 1;
// }

// /* Create the Lin ID parity */
// #define BIT(data,shift) ((ident&(1<<shift))>>shift)
// byte lin_stack::calcIdentParity(byte ident)
// {
//   byte p0 = BIT(ident,0) ^ BIT(ident,1) ^ BIT(ident,2) ^ BIT(ident,4);
//   byte p1 = ~(BIT(ident,1) ^ BIT(ident,3) ^ BIT(ident,4) ^ BIT(ident,5));
//   return (p0 | (p1<<1))<<6;
// }
