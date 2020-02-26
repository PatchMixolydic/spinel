#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <spinel/cpu.h>

uint8_t inByte(uint16_t port);
void outByte(uint16_t port, uint8_t data);
uint16_t inWord(uint16_t port);
void outWord(uint16_t port, uint16_t data);
uint32_t inDWord(uint16_t port);
void outDWord(uint16_t port, uint32_t data);

void loadGDT(uintptr_t loc);
void loadIDT(uintptr_t loc);

uintptr_t getESP(void);

#endif // ndef CPU_H
