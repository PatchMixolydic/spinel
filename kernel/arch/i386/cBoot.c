#include <stdio.h>
#include <kernel/memory.h>
#include <kernel/panic.h>
#include <kernel/tty.h>
#include "gdt.h"
#include "idt.h"
#include "interrupts.h"
#include "memory/paging.h"
#include "pic.h"

void cBoot(multiboot_info_t* mbd, unsigned int magicNum) {
    terminalInitialize();
	terminalDisableCursor();
    printf("Spinel booting for i386.\n");
    initGDT();

	if (magicNum != MULTIBOOT_BOOTLOADER_MAGIC) {
		panic("Invalid bootloader magic number 0x%X", magicNum);
	}
	if ((mbd->flags & MULTIBOOT_INFO_MEM_MAP) == 0) { // memory map not provided
		panic("Bootloader did not provide memory map");
	}

	// We have a valid memory map -- use it
    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)mbd->mmap_addr;
    initPageFrameAllocator(mmap, mbd->mmap_length);
    // We're done with the multiboot struct -- time to cut the identity mapping
    improveKernelPageStructs();

    picSetAllMasked(true);
    picInitialize(PICMasterOffset, PICSubservientOffset);
    initIDT();
    enableInterrupts();
    picSetIRQMasked(0, false);
    picSetIRQMasked(1, false);
}
