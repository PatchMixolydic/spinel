#ifndef SPINEL_VIRTUALMEMORY_H
#define SPINEL_VIRTUALMEMORY_H

#include <sys/types.h>
#include <spinel/kernelInfo.h>

typedef enum {
    VMemNoPerm = 0,
    VMemRead = 1,
    VMemWrite = 2,
    VMemExec = 4
} VMemFlags;

// Map a page such that the given address will be accessible
// This doesn't necessarily create backing memory if the virtual
// memory system is capable of allocating that upon access
// (ex. like with i386/amd64's page fault mechanism)
int vMemMapPage(void* addr, unsigned flags);
// Map pages such that at least the interval [start, end) will be
// accessible
// To ensure that unmapping works as expected, please page align
// start and end
int vMemMapInRange(void* start, void* end, unsigned flags);
// Unmap the page containing the given address
int vMemUnmapPage(void* addr);
// Unmap pages such that at least [start, end) will be inaccessible
int vMemUnmapInRange(void* start, void* end);

#endif // ndef SPINEL_VIRTUALMEMORY_H
