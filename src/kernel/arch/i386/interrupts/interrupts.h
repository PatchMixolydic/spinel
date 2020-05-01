#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

// This header serves two source files:
// interrupts.c, the implementation of interrupt handlers
// isr.asm, the actual interrupt service routines that the CPU calls

typedef enum {
    IntDivisionByZero, IntDebug, IntNMI,
    IntBreakpoint, IntOverflow, IntBoundRangeExceeded,
    IntInvalidOpcode, IntDeviceNotAvailable, IntDoubleFault,
    IntCoprocessorSegmentOverrun, IntInvalidTSS, IntSegmentNotPresent,
    IntStackSegmentFault, IntGeneralProtectionFault, IntPageFault,
    IntReserved0, IntFPUException, IntAlignmentCheck,
    IntMachineCheck, IntSIMDFloatException, IntVirtualizationException,
    IntReserved1, IntSecurityException, IntReserved2,
    IntIRQ0 = 32, IntIRQ1, IntIRQ2, IntIRQ3, IntIRQ4, IntIRQ5, IntIRQ6,
    IntIRQ7, IntIRQ8, IntIRQ9, IntIRQ10, IntIRQ11, IntIRQ12, IntIRQ13,
    IntIRQ14, IntIRQ15,
    IntSyscall = 0x88
} Interrupt;

typedef struct {
    uint32_t interruptNum;
    uint32_t edi, esi, ebp, esp;
    uint32_t ebx, edx, ecx, eax;
    uint32_t errorCode, eip, cs, eflags;
    // The following are in usermode only
    uint32_t userESP, userSS;
} InterruptInfo;

typedef void(*IRQHandler)(void);

void registerInterruptHandler(Interrupt interrupt, IRQHandler handler);
InterruptInfo* getInterruptInfo(void);

#endif // ndef INTERRUPTS_H
