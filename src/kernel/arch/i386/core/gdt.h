#ifndef GDT_H
#define GDT_H

#include <stdint.h>

typedef enum {
    GDTNull = 0,
    GDTKernelCode,
    GDTKernelData,
    GDTUserCode,
    GDTUserData,
    GDTTSS
} GDTEntry;

void initGDT(void);
uint16_t getGDTOffset(GDTEntry entry);
void setTSSKernelStack(uint32_t esp0);

#endif // ndef GDT_H
