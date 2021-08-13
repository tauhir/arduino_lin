/*  LIN network Slave 
 *  
 *
 *  Short description: 
 *  Listens for headers with a specific id
 *  Currently for use with Arduino Mega 2560/Uno
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
unsigned int data_size = 8; // length of byte array
int data[]= {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}; // byte array for received data
int int_array_size = sizeof(data[0]);
byte id = 0x12;

// Creating LIN Object
lin_stack LIN1(1,id,9200); // 1 - channel, ident - Identification Byte

void setup() {
  // Configure Serial communication
  Serial.begin(19200); // Configure Serial for Serial Monitor
  Serial.println("Starting!");
  LIN1.setSerial(); // Configure Serial for receiving
}

void loop() {
  // Checking LIN Bus periodicly
  int a = LIN1.read(data, data_size, true, false);
  if(a == 1){ // If there was an event on LIN Bus, Traffic was detected. Print data to serial monitor

    for(int i=0; i <11;i++){
      if (data[i] != -1){
        Serial.print(data[i]);
        if(i < (10)) {
            Serial.print(", ");
        }
      }
		}
    Serial.println(" ");

    //code below tests that master_test.ino produces the correct results when sniffing
    // int value = 0;
    // boolean result = false;
    // for(int i=0; i <11;i++){
    //   if (data[i] != -1){
    //     value = value + data[i];
    //   }
		// }
    // for (int x=0; x<4; x++){
    // if (checklist[x] == value){
    //     result = true;
    //   }
    // }
    // if (result==false){
    //   Serial.println("false");
    //   for(int i=0; i <11;i++){
    //     if (data[i] != -1){
    //       Serial.print(data[i]);
    //       if(i < (10)) {
    //           Serial.print(", ");
    //       }
    //     }
    //   }
    //   Serial.println(" ");
    // }
    // else {
    //   Serial.println("true");
    // }
  }
  else if(a == -1){ // Ident and Checksum validation Failed
    //Serial.println("Corrupt Request Received!");
  }
  else if(a == -2){
    // getting data but for the wrong ID
     Serial.println("Specified ID enabled, data received but wrong ID!");
  }
}
