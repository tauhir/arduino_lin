/*  LIN STACK for MCP2004
 *  v2.0
 *
 *  Short description: 
 *  Comunication stack for LIN and MCP2004 LIN transceiver. Built off of https://github.com/macchina/LIN/
 *  Currently for use with Arduino Mega 2560
 *  
 *  Author: Tauhir Edwards
 *  Arduino IDE 1.6.9
 * 
 * Notes:
 * I've decided to work with ints instead of bytes except when writing and possibly showing the values on a serial port. 
 * The reason for this is to handle varying data types
 * 
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

lin_stack::lin_stack(byte Ch, int baud=9600){
	baud_rate = baud;
	sleep_config(Ch); // Configurating Sleep pin for transceiver
	identByte = 0;
	ch = Ch;
	buffer_delay = 18; // TODO manually calculated value below.. it doesn't work when I calculate it for some odd reason so this needs to be set if baud not 9600
	//buffer_delay = round(140*(1000/baud)); // at 9200bps, thats 0.1ms per bit. LIN message has 101 bits max with 40% waiting time (at the very max) ~ 15ms for a LIN message and delay from slave
	//Serial.println(buffer_delay);
}

lin_stack::lin_stack(byte Ch, byte ident, int baud=9600){
	baud_rate = baud;
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
	//sleep(1); // Go to Normal mode
	// Synch Break
	//serial_pause(13);
	// Send data via Serial interface
	if(ch==1){ // For LIN1 or LINBusSerialOne
		LINBusSerialOne.begin(baud_rate); // config Serial
		LINBusSerialOne.write(0x55); // write Synch Byte to serial
		LINBusSerialOne.write(ident); // write Identification Byte to serial
		for(int i=0;i<data_size;i++) LINBusSerialOne.write(data[i]); // write data to serial
		LINBusSerialOne.write(checksum); // write Checksum Byte to serial
		LINBusSerialOne.end(); // clear Serial config
	}else if(ch==2){ // For LIN2 or LINBusSerialTwo
		LINBusSerialTwo.begin(baud_rate); // config Serial
		LINBusSerialTwo.write(0x55); // write Synch Byte to serial
		LINBusSerialTwo.write(ident); // write Identification Byte to serialv
		for(int i=0;i<data_size;i++) LINBusSerialTwo.write(data[i]); // write data to serial
		LINBusSerialTwo.write(checksum);// write Checksum Byte to serial
		LINBusSerialTwo.end(); // clear Serial config
	}
	//sleep(0); // Go to Sleep mode
	return 1;
}

int lin_stack::writeForced( byte data[], byte data_size){
	// need this to emulate a slave 
	if(ch==1){ // For LIN1 or LINBusSerialOne
		LINBusSerialOne.begin(baud_rate); // config Serial
		for(int i=0;i<data_size;i++) LINBusSerialOne.write(data[i]); // write data to serial
		LINBusSerialOne.end(); // clear Serial config
	}else if(ch==2){ // For LIN2 or LINBusSerialTwo
		LINBusSerialTwo.begin(baud_rate); // config Serial
		LINBusSerialTwo.write(data[0]);  // write Identification Byte to serialv
		for(int i=0;i<data_size;i++) LINBusSerialTwo.write(data[i]); // write data to serial
		LINBusSerialTwo.end(); // clear Serial config
	}
	//sleep(0); // Go to Sleep mode
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
		LINBusSerialOne.begin(baud_rate); // config Serial
		LINBusSerialOne.write(header,2); // write data to serial
		LINBusSerialOne.end(); // clear Serial config
	}else if(ch==2){ // For LIN2 or LINBusSerialTwo
		LINBusSerialTwo.begin(baud_rate); // config Serial
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
		LINBusSerialOne.begin(baud_rate); // config Serial
		LINBusSerialOne.write(data, data_size); // write data to serial
		LINBusSerialOne.write(checksum); // write data to serial
		LINBusSerialOne.end(); // clear Serial config
	}else if(ch==2){ // For LIN2 or LINBusSerialTwo
		LINBusSerialTwo.begin(baud_rate); // config Serial
		LINBusSerialTwo.write(data, data_size); // write data to serial
		LINBusSerialTwo.write(checksum); // write data to serial
		LINBusSerialTwo.end(); // clear Serial config
	}
	sleep(0); // Go to Sleep mode
	return 1;
}

// int lin_stack::writeStream(byte data[], byte data_size){
// 	// Start interface
// 	sleep(1); // Go to Normal mode
// 	// Synch Break
// 	serial_pause(13);
// 	// Send data via Serial interface
// 	if(ch==1){ // For LIN1 or LINBusSerialOne
// 		LINBusSerialOne.begin(baud_rate); // config Serial
// 		for(int i=0;i<data_size;i++) LINBusSerialOne.write(data[i]);
// 		LINBusSerialOne.end(); // clear Serial config
// 	}else if(ch==2){ // For LIN2 or LINBusSerialTwo
// 		LINBusSerialTwo.begin(baud_rate); // config Serial
// 		for(int i=0;i<data_size;i++) LINBusSerialTwo.write(data[i]);
// 		LINBusSerialTwo.end(); // clear Serial config
// 	}
// 	sleep(0); // Go to Sleep mode
// 	return 1;
// }

// READ methods
// Read LIN traffic and then process it.
int lin_stack::setSerial(){ // Only needed when receiving signals
	Serial.begin(19200);
	if(ch==1){ // For LIN1 (Channel 1)
		LINBusSerialOne.begin(baud_rate); // Configure LINBusSerialOne
	} else if(ch==2){ // For LIN2 (Channel 2)
		LINBusSerialTwo.begin(baud_rate); // Configure LINBusSerialTwo
	}
}

int lin_stack::read(
	int data[],
 	int data_size,
	boolean all_data = true,
	boolean id_specific = true,
	boolean slave = false
	){
	// added all_data to instead of just providing data, send back all values (sync, id, checksum etc)
	// id_specific when false will read everything and not check parity.
	// for now, data_size should just be 8
	
	//issue - what do we do if we connect and the first value is not a sync byte for some reason? I think ignoring current command and listening to the next is fine
	int total_bytes = data_size+3;
	int rec[]= {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}; //this should work for now
	unsigned int counter = 0; 
	int throw_away = 0;
	int peek= 0;
	String h = "";
	boolean sync_found = false;
	if(ch==1){ // For LIN1 or LINBusSerialOne;
		while( (LINBusSerialOne.available() > 0) & (counter < total_bytes)) {

			peek = LINBusSerialOne.peek();
			//Serial.println(peek);
			if (counter == 0){
				if (peek == 85) {
					rec[counter] = LINBusSerialOne.read();
					sync_found = true;
					counter++;
				}
				else {
					throw_away = LINBusSerialOne.read();
				}	
			}
			else if (peek == 85) {
				//instead of getting data from the serial buffer, write a negative value 
				counter = total_bytes;
			}
			// else if (counter == 1) {
			// 	if (slave == true){
			// 		if (identByte == 0) {
			// 			return -1;
			// 		}
			// 		if (peek = identByte){
			// 			return 2;
			// 		}
			// 		else {
			// 			return 0;
			// 		}
			// 	}
			// }
			else {
				rec[counter] = LINBusSerialOne.read();
				counter++;
			}
		}
	}
	// else if(ch==2){ // For LIN2 or LINBusSerialTwo
	// 	while( (LINBusSerialTwo.available() > 0) & (counter < 7)) {
	// 		if (int(LINBusSerialTwo.peek()) == 85) {
	// 			//instead of getting data from the serial buffer, write a negative value 
	// 			for (int i = counter;i<7;i++){
	// 				rec[i]= -1;
	// 			}
	// 			counter = 7;
	// 		}
	// 		else {
	// 			rec[counter] = LINBusSerialTwo.read();
	// 		}
	// 	}
	// }
	// now we have an array of size 11 at all times, with -1s at the end to be ignored TODO: instead use the id range to determine data size

	//check if we've got the ID we're after
	// check if id is valid
	// validate checksum 
	h = " ";
    delay(buffer_delay); //seems like i need to wait for the buffer to fill

	if (rec[0] != 85) {
		//first byte isn't sync byte - BAD DATA
		
		return -1;
	}
	
	if(int(rec[2]) == -1) {
		return -1;
	}
	if(id_specific ==true) {
		if (rec[1] != int(identByte)) {
			return -2;
		}
	}
	if (all_data == true) {
		//maybe work with data the whole time and save some time
		for(int i=0; i <11;i++){
			data[i]=rec[i];
			}
		return 1;
	}
	else { 
		for(int j=0;j<data_size;j++){
			data[j] = rec[j+2];
			
		}
		return 1;
	}

	//if((id_specific==true)&(validateParity(rec[1]))&(validateChecksum(rec,data_size+3))){
	return 0;
}

int lin_stack::sniffBus(int data[], boolean all_data, boolean id_specific){
	//listens to everything. 
	return read(data, 8, all_data, false,false);
}
int lin_stack::sniffSlave(){
	//need to listen for a request
	int tempData[11];
	int response = read(tempData, 8, true, true, true);
	if (response == 2) {
		return 1;
	}
	else if (response == -1) {
		return response;
	}
	else {
		return 0;
	}
}

// PRIVATE METHODS
int lin_stack::serial_pause(int no_bits){
	// Calculate delay needed for 13 bits, depends on bound rate
	//baud rate is bits per second so we need to determine how many ms needed for 13 bits
	// 1000000 milliseconds/ boud_rate gives us bits per millisecond
	unsigned long time_in_micro_sec =(1000000/baud_rate)*no_bits;
	if (time_in_micro_sec > 16383) {
		return -1;
	}
	else {
		delayMicroseconds(time_in_micro_sec);
		return 1;
	}
	return 1;
}

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

boolean lin_stack::validateChecksum( int data[], byte data_size){
	byte checksum = data[data_size-1];
	byte suma = 0;
	for(int i=2;i<data_size-1;i++)
		if (data[i] > 0) {
			suma = suma + data[i];
		} 
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
