#include <SoftwareSerial.h>
//AltSoftSerial portOne; //was testing if library is the issue
int incoming_byte = 0;
SoftwareSerial portOne(10,11); 

void setup() {
  Serial.begin(19200);
  Serial.println("Start receiving");
  portOne.begin(19200);
  pinMode(9, OUTPUT);
  digitalWrite(9, HIGH);
  pinMode(8, OUTPUT);
  digitalWrite(8, HIGH);
  
}

void loop() {


    while (portOne.available() > 0) {
      incoming_byte = portOne.read();
      if (incoming_byte == 85){
        Serial.println(" ");
      }
      Serial.print(incoming_byte);
      Serial.print(" ");
    }
}