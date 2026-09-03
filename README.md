# arduino_lin

Sniff and inject frames on a LIN bus from an Arduino Mega 2560, using an MCP2004 LIN transceiver. The `lin_stack` library is built on [macchina/LIN](https://github.com/macchina/LIN), reworked for SoftwareSerial on the Mega so it doesn't need a Macchina M2.

Hobby project from 2021, built to read a car's LIN bus. It works, but it isn't maintained.

## Hardware

- Arduino Mega 2560
- MCP2004 (or MCP2003/MCP2025) LIN transceiver
- 12 V supply for the transceiver's bus side

| Signal          | Channel 1 | Channel 2 |
|-----------------|-----------|-----------|
| RX (from MCP)   | D10       | D12       |
| TX (to MCP)     | D11       | D13       |
| CS (enable)     | D8        | D6        |
| FAULT           | D9        | D7        |

Bus speed is 9600 baud (`bound_rate` in `lin_stack.h`; the LIN standard is 10417 but most devices tolerate 9600 on SoftwareSerial). Serial monitor runs at 19200.

## Sketches

| Path                     | What it does                                                                                     |
|--------------------------|--------------------------------------------------------------------------------------------------|
| `arduino_lin.ino`        | Main sniffer. Reads every frame on channel 1, validates ident parity and checksum, prints the data bytes to Serial. |
| `bus_sniffer/`           | Earlier sniffer variant that dumps raw 8-byte frames without filtering.                          |
| `master_test/`           | Acts as the LIN master: writes test frames to ident `0x11`. Use this to inject onto the bus.     |
| `src/slave.ino`          | Slave node that only responds to ident `0x11`.                                                   |
| `serial_test/`           | Raw SoftwareSerial dump on D10/D11 to check transceiver wiring before using the stack.           |

Each sketch folder carries its own copy of `lin_stack.cpp/.h` because the Arduino IDE compiles a folder as a unit. The copies drifted slightly during development; `arduino_lin.ino` with the root `lin_stack.*` is the one that was finished.

## lin_stack API

```cpp
lin_stack LIN1(1);          // master on channel 1
lin_stack LIN2(2, 0x11);    // slave on channel 2, ident 0x11

LIN1.setSerial();                                  // start listening
int r = LIN1.read(data, size, all_data, id_only);  // 1 = frame, -1 = bad ident/checksum, 0 = nothing
LIN1.write(0x11, data, size);                      // header + response
LIN1.writeRequest(0x11);                           // header only
LIN1.writeResponse(data, size);                    // response only
LIN1.busWakeUp();
```

## Credits

- [macchina/LIN](https://github.com/macchina/LIN) for the original stack.
- [zapta/linbus](https://github.com/zapta/linbus) for the checksum reference.
