#include <stdint.h>

#define SetVersionNumber(major, minor, patche) \
    const uint32_t SpinelVersionMajor = major;\
    const uint32_t SpinelVersionMinor = minor;\
    const uint32_t SpinelVersionPatch = patche;\
    const char* const SpinelReleaseVersion = #major "." #minor "." #patche;\

SetVersionNumber(0, 1, 0)
const char* const SpinelKernelName = "Spinel";
const char* const SpinelBuildVerison = __DateTime__ " " __Commit__;
