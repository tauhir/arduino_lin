#define RX_PIN 10
#define TX_PIN 11
#define RX_PIN 12
#define TX_PIN 13
#define FAULT_PIN 14
#define CS_PIN 8

#define SYN_FIELD 0x55
#define SWM_ID 0x20

SoftwareSerial LINBusSerial(RX_PIN, TX_PIN);