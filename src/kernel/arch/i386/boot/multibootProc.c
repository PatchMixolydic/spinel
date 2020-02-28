#include <stdint.h>
#include <stdio.h>
#include "../memory/physicalMem.h"
#include "multibootProc.h"

void processMultibootInfo(struct multiboot_tag* info) {
    // go ahead and skip the header
    for (
        info += 8;
        info->type != MULTIBOOT_TAG_TYPE_END;
        // Next power of 2 up from the size
        info = (struct multiboot_tag*)((uint8_t*)info + ((info->size + 7) & ~7))
    ) {
        printf("Got Multiboot tag %d\n", info->type);
        switch (info->type) {
            case MULTIBOOT_TAG_TYPE_MMAP: {
                initPhysicalAlloc(
                    ((struct multiboot_tag_mmap*)info)->entries,
                    info->size - (sizeof(uint32_t) * 4) // size of header
                );
                break;
            }
        }
    }
}
