#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <spinel/cpu.h>

extern uint8_t inByte(uint16_t port);
extern void outByte(uint16_t port, uint8_t val);
extern uint16_t inShort(uint16_t port);
extern void outShort(uint16_t port, uint16_t val);
extern uint32_t inWord(uint16_t port);
extern void outWord(uint16_t port, uint32_t val);

#endif // ndef CPU_H
