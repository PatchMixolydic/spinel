#ifndef _KERNEL_PIC_H
#define _KERNEL_PIC_H

#include <stdbool.h>

static const uint8_t PICMasterOffset = 0x20;
static const uint8_t PICSubservientOffset = 0x28;

/*
 * TODO: this is very IBM PC centric
 * I'm just putting it here to get it working
 * Should probably be moved to a HAL or to arch/i386 to be included in some
 * x86 specific boot code.
 * If I'm refactoring to replace i386 with amd64... now would be a good time ;)
 */

void picInitialize(int masterOffset, int subservOffset);
void picDisable();
void picEndOfInterrupt(bool toSubserv);
uint16_t picGetIRQRegister();
uint16_t picGetInServiceRegister();

#endif // ndef _KERNEL_PIC_H
