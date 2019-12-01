#ifndef ARCH_I386_IO_H
#define ARCH_I386_IO_H

#include <stdint.h>

extern void outb(uint16_t port, uint16_t val);
extern uint16_t inb(uint16_t port);

#endif // ndef ARCH_I386_IO_H
