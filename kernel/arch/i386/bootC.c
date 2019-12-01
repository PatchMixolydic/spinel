#include <stdio.h>
#include "pic.h"

void bootC() {
    printf("Spinel booting for i386.\n");
    picInitialize(PICMasterOffset, PICSubservientOffset);
}
