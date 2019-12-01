#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <kernel/multiboot.h>
#include <kernel/panic.h>
#include <kernel/pic.h>

void kernelMain(multiboot_info_t* mbd, unsigned int magicNum) {
	terminalDisableCursor();
	printf("The system is coming up.\n");

	if (magicNum != MULTIBOOT_BOOTLOADER_MAGIC) {
		panic("Invalid bootloader magic number"); // TODO: provide magic number
	}
	if ((mbd->flags & MULTIBOOT_INFO_MEM_MAP) == 0) { // memory map not provided
		panic("Bootloader did not provide memory map");
	}

	// We have a valid memory map -- use it
	multiboot_memory_map_t* mmap = mbd->mmap_addr;
	while (mmap < mbd->mmap_addr + mbd->mmap_length) {
		uint64_t start = mmap->addr;
		uint64_t end = mmap->addr + mmap->len;
		bool valid = mmap->type == 1;
		// mmap->size fails to account for itself
		mmap = (multiboot_memory_map_t*) ((unsigned int)mmap + mmap->size + sizeof(mmap->size));
	}

	picInitialize(PICMasterOffset, PICSubservientOffset);

	panic("nevermind");
}
