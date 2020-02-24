#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <spinel/cpu.h>

uint8_t inByte(uintptr_t port);
// TODO: see how x86 pads data and change to uint8_t
void outByte(uintptr_t port, uint32_t data);
uint16_t inWord(uintptr_t port);
void outWord(uintptr_t port, uint32_t data);
uint32_t inDWord(uintptr_t port);
void outDWord(uintptr_t port, uint32_t data);

void loadGDT(uintptr_t loc);
void loadIDT(uintptr_t loc);

uintptr_t getESP();

#endif // ndef CPU_H
