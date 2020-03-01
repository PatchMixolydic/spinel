#include <stddef.h>
#include <stdint.h>

// Not sure if this is technically true, but it's probably more accurate than
// redoubling "i386"
// Also, if I port this to a non-IBM PC x86 architecture later, (PC-98 maybe?),
// this will help disambiguate between them
// Also also, similar tactics will probably be *very* useful in the event of
// an ARM or RISC-V port
const char* const MachineName = "PC compatible";
const char* const ProcessorName = "i386";

const size_t PageSize = 4096;
const uintptr_t KernelOffset = 0xC0000000;
const uintptr_t KernelHeapStart = 0xD0000000;
const uintptr_t KernelHeapEnd = 0xE0000000;
