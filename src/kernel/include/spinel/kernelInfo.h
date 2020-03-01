#ifndef SPINEL_KERNELINFO_H
#define SPINEL_KERNELINFO_H

#include <stdint.h>

/* For source readers and kernel developers:
 * This header encompasses architecture-specific and general info
 * Architecture specific info should probably be placed in
 * arch/{arch}/core/kernelInfo.c
 */

// Version numbers
extern const uint32_t SpinelVersionMajor;
extern const uint32_t SpinelVersionMinor;
extern const uint32_t SpinelVersionPatch;

// Version info
extern const char* const SpinelKernelName;
// Based on uname's version field
// Seems to generally contain build date, commit number
extern const char* const SpinelBuildVerison;
// Release version:
// Based on uname's release field
// Typically a version number, sometimes with a hash
// This would be Major.Minor.Patch for Spinel

// Architecture-dependent version info
extern const char* const SpinelMachineName;
extern const char* const SpinelProcessorName;

// Architecture-dependent
extern const size_t PageSize;
extern const uintptr_t KernelOffset;
extern const uintptr_t KernelHeapStart;
extern const uintptr_t KernelHeapEnd;

#endif // ndef SPINEL_KERNELINFO_H