#ifndef _LIBC_ASSERT_H
#define _LIBC_ASSERT_H

#include <stdio.h>

#ifndef NDEBUG
    #ifdef __Kernel
        #define assert(expr) do {\
            if (!(expr)) {\
                panic(\
                    "Assertion failed at %s:%d (%s):\n%s\n",\
                    __FILE__, __LINE__, __func__, #expr\
                );\
            }\
        } while (0)
    #else
        #define assert(expr) do {\
            if (!(expr)) {\
                printf(\
                    "Assertion failed at %s:%d (%s):\n%s\n",\
                    __FILE__, __LINE__, __func__, #expr\
                );\
                abort();\
            }\
        } while (0)
    #endif
#else
    #define assert(expr)
#endif

#endif // ndef _LIBC_ASSERT_H
