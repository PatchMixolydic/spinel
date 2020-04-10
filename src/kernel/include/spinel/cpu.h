#ifndef SPINEL_CPU_H
#define SPINEL_CPU_H

#include <stdint.h>

void haltCPU(void);
void enableInterrupts(void);
void disableInterrupts(void);
void printBacktrace(void);

#endif // ndef SPINEL_CPU_H
