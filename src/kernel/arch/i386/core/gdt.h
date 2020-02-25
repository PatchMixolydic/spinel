#ifndef GDT_H
#define GDT_H

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

#endif // ndef GDT_H
