#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

#define SerialPort1 0x3F8
#define SerialPort2 0x2F8
#define SerialPort3 0x3E8
#define SerialPort4 0x2E8

extern const unsigned SerialMaxBaudRate;

void initSerial(unsigned baudRate);
void setSerialBaudRate(uint16_t serialPort, unsigned baudRate);
uint8_t serialRead(uint16_t serialPort);
void serialWrite(uint16_t serialPort, uint8_t val);
void serialWriteStrLen(uint16_t serialPort, const char str[], size_t len);
void serialWriteStr(uint16_t serialPort, const char str[]);

#endif // ndef SERIAL_H
