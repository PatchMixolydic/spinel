#ifndef SPINEL_ARCHINFO_H
#define SPINEL_ARCHINFO_H

#include <stdint.h>

extern const char* const ArchName;
extern const size_t PageSize;
extern const uintptr_t KernelOffset;
extern const uintptr_t KernelHeapStart;
extern const uintptr_t KernelHeapEnd;

#endif // ndef SPINEL_ARCHINFO_H
