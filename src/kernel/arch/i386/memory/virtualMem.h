#ifndef VIRTUALMEM_H
#define VIRTUALMEM_H

#include "../interrupts/interrupts.h"

void setupPageMaps();
void handlePageFault(InterruptInfo info);

#endif // ndef VIRTUALMEM_H
