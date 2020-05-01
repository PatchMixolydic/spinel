#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include "cpu.h"
#include "fpu.h"

// Disables FWait in the event of emulated FPU instructions
static const uint32_t FPUDisableFWait = 2;
static const uint32_t FPUEmulate = 4;
// When set, all floating point operations will throw Device Not Available,
// allowing the CPU to back up FPU data lazily.
static const uint32_t FPUTaskSwitched = 8;
static const uint32_t FPUIs387Compatible = 16;
static const uint32_t FPUNativeException = 32;
static const uint32_t FPUEnableSSE = 512;
// When this flag is cleared, SSE instructions will stop working
// once an FPU exception has been thrown
static const uint32_t FPUNotDisableSSEOnExcept = 1024;
static const uint32_t FPUEnableXSave = 1 << 18;

static bool fpuInitialized = false;
static uint8_t defaultFPUState[FPUStateSize];

void initFPU(void) {
    // TODO: check FPU cpuid bit
    uintptr_t cr0 = getCR0();
    if ((cr0 & FPUEmulate) && !(cr0 & FPUIs387Compatible)) {
        // Oops, FPU emulation is required
        // TODO: is this right?
        printf("CR0.EM was already set, defaulting to software emulation\n");
        printf("(which doesn't exist right now)\n");
        return;
    }
    // Initialize FPU
    __asm__ ("fninit");
    // TODO: cr0 should be set to enable FPUNativeException
    // setCR0 seems to freeze with no discernable reason
    // TODO: SSE

    // Save the default state
    __asm__ ("fsave %0" : "=m" (defaultFPUState));
}

uint8_t* fpuGetDefaultState(void) {
    assert(fpuInitialized);
    return defaultFPUState;
}
