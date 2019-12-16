__KernelOffset = 0xC0000000;

/* Entry point */
ENTRY(_start)

SECTIONS {
	/* Locate kernel at 0xC0000000 + 1 Mib, load at 1 MiB */
	. = 1M + __KernelOffset;
	__KernelStart = .;

	/* Multiboot header and code */
	.text BLOCK(4K) : AT(ADDR(.text) - __KernelOffset) {
		*(.multiboot)
		*(.text)
	}
	__TextStart = ADDR(.text);
	__TextEnd = .;

	/* Read-only data */
	.rodata BLOCK(4K) : AT(ADDR(.rodata) - __KernelOffset) {
		*(.rodata)
	}
	__RODataStart = ADDR(.rodata);
	__RODataEnd = .;

	/* Read-write data (initialized) */
	.data BLOCK(4K) : AT(ADDR(.data) - __KernelOffset) {
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : AT(ADDR(.bss) - __KernelOffset) {
		*(COMMON)
		*(.bss)
	}

	/* Mark the end of memory for use in the memory manager */
	__KernelEnd = .;
}
