#ifndef ARCH_I386_IO_H
#define ARCH_I386_IO_H

#include <stdint.h>

extern uint8_t inByte(uint16_t port);
extern void outByte(uint16_t port, uint8_t val);
extern uint16_t inShort(uint16_t port);
extern void outShort(uint16_t port, uint16_t val);
extern uint32_t inWord(uint16_t port);
extern void outWord(uint16_t port, uint32_t val);

#endif // ndef ARCH_I386_IO_H
