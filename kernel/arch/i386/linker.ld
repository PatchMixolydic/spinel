/* Entry point */
ENTRY(_start)

SECTIONS {
	/* Load kernel at 1 MiB */
	. = 1M;
	kernelBegin = .;

	/* Multiboot header and code */
	.text BLOCK(4K) : ALIGN(4K) {
		*(.multiboot)
		*(.text)
	}

	/* Read-only data */
	.rodata BLOCK(4K) : ALIGN(4K) {
		*(.rodata)
	}

	/* Read-write data (initialized) */
	.data BLOCK(4K) : ALIGN(4K) {
		*(.data)
	}

	/* Read-write data (uninitialized) and stack */
	.bss BLOCK(4K) : ALIGN(4K) {
		*(COMMON)
		*(.bss)
	}

	/* Mark the end of memory for use in the memory manager */
	kernelEnd = .;
}
