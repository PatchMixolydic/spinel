#include <stdio.h>
#include <kernel/memory.h>
#include <kernel/panic.h>
#include "idt.h"
#include "interrupts.h"
#include "pic.h"

void cBoot(multiboot_info_t* mbd, unsigned int magicNum) {
    printf("Spinel booting for i386.\n");

    for (int i = 0; i <= 15; i++) {
        picSetIRQMasked(i, true); // mask all irqs for now
    }

	if (magicNum != MULTIBOOT_BOOTLOADER_MAGIC) {
        printf("Magic number 0x%X?!\n", magicNum);
		panic("Invalid bootloader magic number"); // TODO: vararg panic
	}
	if ((mbd->flags & MULTIBOOT_INFO_MEM_MAP) == 0) { // memory map not provided
		panic("Bootloader did not provide memory map");
	}

	// We have a valid memory map -- use it
    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mbd->mmap_addr;
    initMemoryManager(mmap, mbd->mmap_length);

    picInitialize(PICMasterOffset, PICSubservientOffset);
    initIDT();
    enableInterrupts();
    picSetIRQMasked(1, false);
}
