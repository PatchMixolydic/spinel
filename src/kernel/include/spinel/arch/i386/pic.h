#ifndef PIC_H
#define PIC_H

#include <stdbool.h>
#include <stdint.h>

void initPIC(void);
void picDisable(void);
void picEndOfInterrupt(uint8_t irq);
uint16_t picGetIRQRegister(void);
uint16_t picGetInServiceRegister(void);
void picSetIRQMasked(uint8_t irq, bool masked);
void picSetAllMasked(bool masked);

#endif // ndef PIC_H
