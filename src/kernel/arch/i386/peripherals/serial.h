#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>

extern const uint16_t SerialPort1;
extern const uint16_t SerialPort2;
extern const uint16_t SerialPort3;
extern const uint16_t SerialPort4;

extern const unsigned SerialMaxBaudRate;

void initSerial(unsigned baudRate);
void setSerialBaudRate(uint16_t serialPort, unsigned baudRate);
uint8_t serialRead(uint16_t serialPort);
void serialWrite(uint16_t serialPort, uint8_t val);
void serialWriteStrLen(uint16_t serialPort, const char str[], size_t len);
void serialWriteStr(uint16_t serialPort, const char str[]);

#endif // ndef SERIAL_H
