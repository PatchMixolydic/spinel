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
uintptr_t getEBP(void);
void setCR0(uintptr_t cr0);
uintptr_t getCR0(void);
uintptr_t getCR2(void);
void setCR3(uintptr_t map);
uintptr_t getCR3(void);

void invalidatePage(void* page);

#endif // ndef CPU_H
