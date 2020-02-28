#include <stddef.h>
#include <stdint.h>

const char* const ArchName = "i386";
const size_t PageSize = 4096;
const uintptr_t KernelOffset = 0xC0000000;
const uintptr_t KernelHeapStart = 0xD0000000;
const uintptr_t KernelHeapEnd = 0xE0000000;
