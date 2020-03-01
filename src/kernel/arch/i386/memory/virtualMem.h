#ifndef VIRTUALMEM_H
#define VIRTUALMEM_H

#include "../interrupts/interrupts.h"

void setupPageMaps(void);
void mapPage(uintptr_t virtual, uintptr_t flags);
void unmapPage(uintptr_t virtual);
void handlePageFault(InterruptInfo info);

#endif // ndef VIRTUALMEM_H
