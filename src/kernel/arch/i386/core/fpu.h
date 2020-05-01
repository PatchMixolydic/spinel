#ifndef FPU_H
#define FPU_H

// TODO: should be 512 with XSave support
#define FPUStateSize 108

void initFPU(void);
uint8_t* fpuGetDefaultState(void);

#endif // ndef FPU_H
