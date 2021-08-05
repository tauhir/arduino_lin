#include <AltSoftSerial.h>
AltSoftSerial portOne;
int incoming_byte = 0;

void setup() {
  Serial.begin(19200);
  Serial.println("Start receiving");
  portOne.begin(19200);
  
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