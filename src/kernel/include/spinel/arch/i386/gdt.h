#ifndef GDT_H
#define GDT_H

#include <stdbool.h>
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
void loadTSS(bool userMode);
uint16_t getGDTSelector(GDTEntry entry);
void setTSSKernelStack(uint32_t esp0);

#endif // ndef GDT_H
