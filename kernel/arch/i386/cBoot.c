#include <stdio.h>
#include "idt.h"
#include "interrupts.h"
#include "pic.h"

void cBoot() {
    printf("Spinel booting for i386.\n");

    for (int i = 0; i <= 15; i++) {
        picSetIRQMasked(i, true); // mask all irqs for now
    }

    picInitialize(PICMasterOffset, PICSubservientOffset);
    initIDT();
    enableInterrupts();
    picSetIRQMasked(1, false);
}
