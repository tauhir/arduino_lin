/*  Sniffer/Reader for Lin Netwrok
 *  v2.0
 *
 *  Short description: 
 *  Sends all LIN communication to the serial port
 *  Currently for use with Arduino Mega 2560
 *  
 *  Author: Tauhir Edwards
 *  Arduino IDE 1.8.15
*/ 

// Including LIN Stack library
#include "lin_stack.h"
// Including NonDuePin library for controlling LEDs
//#define FAULT_PIN_ONE 9
//#define CS_PIN_ONE 8
//#define FAULT_PIN_TWO 7
//#define CS_PIN_TWO 6
//rx,tx for lin 1: pin 10 & 11
//rx, tx for lin 2: pin 12,13
// Variables
// const byte ident = 0x11; // Identification Byte
byte data_size=4; // length of byte array
byte data[4]; // byte array for received data


// Creating LIN Object
lin_stack LIN1(1); // 1 - channel, ident - Identification Byte

void setup() {
  // Configure Serial communication
  Serial.begin(19200); // Configure Serial for Serial Monitor
  Serial.println("Starting!");
  LIN1.setSerial(); // Configure Serial for receiving
}

void loop() {
  // Checking LIN Bus periodicly
  byte a = LIN1.read(data, data_size, true, false);
  if(a == 1){ // If there was an event on LIN Bus, Traffic was detected. Print data to serial monitor

    Serial.println("Request Received!");
    int array_length = sizeof(data) - 1;
    for(int i=0; i <= array_length;i++){
			Serial.print(data[i]);
        if(i < array_length) {
            Serial.print(",");
        }
			} 
  }else if(a == -1){ // Ident and Checksum validation Failed
    Serial.println("Corrupt Request Received!");
  }
}
