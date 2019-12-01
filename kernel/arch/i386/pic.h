#ifndef _KERNEL_PIC_H
#define _KERNEL_PIC_H

#include <stdbool.h>
#include <stdint.h>

static const uint8_t PICMasterOffset = 0x20;
static const uint8_t PICSubservientOffset = 0x28;

void picInitialize(int masterOffset, int subservOffset);
void picDisable();
void picEndOfInterrupt(bool toSubserv);
uint16_t picGetIRQRegister();
uint16_t picGetInServiceRegister();

#endif // ndef _KERNEL_PIC_H
