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
// For other operating systems,
// seems to generally contain build date, commit id
extern const char* const SpinelBuildVerison;
// Release version:
// Based on uname's release field
// For other operating systems,
// typically a version number, sometimes with a commit id
extern const char* const SpinelReleaseVersion;

// Architecture-dependent version info
// Machine name should represent the hardware platform
// Contrary to common uname practices, this is not the processor
// (unless the processor defines the platform)
// If there are multiple processor architectures for a given hardware platform
// (like the PC compatible), they should be disambiguated by ProcessorName.
// Examples:
// PC compatible, PC-98, Amiga, PinePhone, HiFive Unleashed, Sharp X68000
extern const char* const MachineName;
extern const char* const ProcessorName;

// Architecture-dependent
extern const size_t PageSize;
extern const uintptr_t KernelOffset;
extern const uintptr_t KernelHeapStart;
extern const uintptr_t KernelHeapEnd;

#endif // ndef SPINEL_KERNELINFO_H
