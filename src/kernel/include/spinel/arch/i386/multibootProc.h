#ifndef MULTIBOOTPROC_H
#define MULTIBOOTPROC_H

#include <multiboot2.h>

// Multiboot processing
// Initializes the physical memory manager
// TODO: is this too strange?
void processMultibootInfo(struct multiboot_tag* info);

#endif // ndef MULTIBOOTPROC_H
