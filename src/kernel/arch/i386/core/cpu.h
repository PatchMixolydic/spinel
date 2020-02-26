#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <spinel/cpu.h>

void loadGDT(uintptr_t loc);
void loadIDT(uintptr_t loc);

uintptr_t getESP(void);

// I can't get these to work in NASM

static inline uint8_t inByte(uint16_t port) {
    uint8_t res;
    __asm volatile ("inb %1, %0" : "=a"(res) : "Nd"(port));
    return res;
}

static inline void outByte(uint16_t port, uint8_t val) {
    __asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint16_t inWord(uint16_t port) {
    uint16_t res;
    __asm volatile ("inw %1, %0" : "=a"(res) : "Nd"(port));
    return res;
}

static inline void outWord(uint16_t port, uint16_t val) {
    __asm volatile ("outw %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint32_t inDWord(uint16_t port) {
    uint32_t res;
    __asm volatile ("ind %1, %0" : "=a"(res) : "Nd"(port));
    return res;
}

static inline void outDWord(uint16_t port, uint32_t val) {
    __asm volatile ("outd %0, %1" : : "a"(val), "Nd"(port));
}

#endif // ndef CPU_H
