#include <stdio.h>
#include "pic.h"

void bootC() {
    printf("nyaaOS booting for i386.\n");
    picInitialize(PICMasterOffset, PICSubservientOffset);
}
