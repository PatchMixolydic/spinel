#include <stdint.h>

#ifdef __Kernel
    #include <spinel/panic.h>
#else
    #include <stdio.h>
    #include <stdlib.h>
#endif

#if UINTPTR_MAX == UINT64_MAX
    uintptr_t __stack_chk_guard = 0xDEADBEEF0BADCAFE;
#else
    uintptr_t __stack_chk_guard = 0xDEADBEEF;
#endif

void __stack_chk_fail(void) {
    #ifdef __Kernel
        panic("Stack smashing detected");
    #else
        // TODO: stderr
        printf("Stack smashing detected");
        // TODO: abort
    #endif
}
