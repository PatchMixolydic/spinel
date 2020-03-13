#include <stddef.h>
#include <stdint.h>

// This probably isn't technically correct anymore, but this is probably the
// best name for the modern x86 computer architecture. Amigas can be PCs, but
// they aren't IBM PC compatibles.
const char* const MachineName = "PC compatible";
// Also extant: 486, 586, 686. Might be worth it to use x86, but it's kind of
// funny to call a 686 (or perhaps a Threadripper running 32-bit Spinel) a 386.
const char* const ProcessorName = "i386";

const size_t PageSize = 4096;
// From linker script
const uintptr_t KernelOffset = __KernelStart;
const uintptr_t KernelHeapStart = 0xD0000000;
const uintptr_t KernelHeapEnd = 0xE0000000;
