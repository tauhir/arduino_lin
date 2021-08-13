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

#include <Arduino.h>
#include <SoftwareSerial.h>
/*
	Please, read Getting Started Guide firts.
*/

class lin_stack
{
	public:
		// Constructors
		lin_stack(byte Ch,int baud); // Constructor for Master Node
		lin_stack(byte Ch, byte ident,int baud); // Constructor for Slave Node
		
		// Methods
		int rx_one = 10;
		int tx_one = 11;
		int rx_two = 12;
		int tx_two = 13;
		// Writing data to bus
		int write(byte add, byte data[], byte data_size); // write whole package
		int writeForced( byte data[], byte data_size);
		int writeRequest(byte add); // Write header only
		int writeResponse(byte data[], byte data_size); // Write response only
		int writeStream(byte data[], byte data_size); // Writing user data to LIN bus
		int read(int data[], int data_size,boolean all_data, boolean id_specific, boolean slave); // read data from LIN bus, checksum and ident validation
		int setSerial(); // set up Seril communication for receiving data.
		int busWakeUp(); // send wakeup frame for waking up all bus participants
		int slavelisten(); //listens for ident and returns a int to action accordingly
		int sniffBus(int data[], boolean all_data, boolean id_specific); //sniffs entire LIN BUS
		int sniffSlave();
	
	// Private methods and variables
	private:
		unsigned int baud_rate; // 10417 is best for LIN Interface, most device should work
		const unsigned int period = 52; // in microseconds, 1s/10417
		unsigned long buffer_delay;
		byte ch = 0; // which channel should be used
		byte identByte; // user defined Identification Byte
		int sleep(byte sleep_state); // method for controlling transceiver modes (0 - sleep, 1 - normal)
		int sleep_config(byte serial_No); // configuration of sleep pins
		int serial_pause(int no_bits); // for generating Synch Break
		boolean validateParity(byte ident); // for validating Identification Byte, can be modified for validating parity
		boolean validateChecksum(int data[], byte data_size); // for validating Checksum Byte
		byte calcIdentParity(byte ident);
		
};
