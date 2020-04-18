#ifndef _LIBC_SYS_ENDIAN_H
#define _LIBC_SYS_ENDIAN_H

#include <stdint.h>

// Check if endianness is supported
#ifdef __BYTE_ORDER__
    #if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__ &&\
        __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__
        #error "PDP endianness is not supported!"
    #endif
#else
    #error "Can't determine byte ordering in sys/endian.h"
#endif

// Catches GCC and Clang
#ifdef __GNUC__
    // GCC and Clang have built-in byte swapping functions
    // that can compile to dedicated instructions if they're
    // faster
    // Also, inline functions are C99, so it's probably not
    // wise to use them here... some people are C89 fans

    #define bswap16(x) __builtin_bswap16(x)
    #define bswap32(x) __builtin_bswap32(x)
    #define bswap64(x) __builtin_bswap64(x)
#else
    #define bswap16(x) ((((x) & 0xFF) << 8) | (((x) & 0xFF00) >> 8))

    #define bswap32(x) ((((x) & 0xFF) << 24) | (((x) & 0xFF00) << 8) |\
        (((x) & 0xFF0000) >> 8) | (((x) & 0xFF000000) >> 24))

    #define bswap64(x) ((((x) & 0xFF) << 56) |\
        (((x) & 0xFF00) << 40) |\
        (((x) & 0xFF0000) << 24) |\
        (((x) & 0xFF000000) << 8) |\
        (((x) & 0xFF00000000) >> 8) |\
        (((x) & 0xFF0000000000) >> 24) |\
        (((x) & 0xFF000000000000) >> 40) |\
        (((x) & 0xFF00000000000000) >> 56))
#endif

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define htole16(x) ((uint16_t)(x))
    #define htole32(x) ((uint32_t)(x))
    #define htole64(x) ((uint64_t)(x))
    #define le16toh(x) ((uint16_t)(x))
    #define le32toh(x) ((uint32_t)(x))
    #define le64toh(x) ((uint64_t)(x))

    #define htobe16(x) bswap16(x)
    #define htobe32(x) bswap32(x)
    #define htobe64(x) bswap64(x)
    #define be16toh(x) bswap16(x)
    #define be32toh(x) bswap32(x)
    #define be64toh(x) bswap64(x)
#else // Big endian
    #define htobe16(x) ((uint16_t)(x))
    #define htobe32(x) ((uint32_t)(x))
    #define htobe64(x) ((uint64_t)(x))
    #define be16toh(x) ((uint16_t)(x))
    #define be32toh(x) ((uint32_t)(x))
    #define be64toh(x) ((uint64_t)(x))

    #define htole16(x) bswap16(x)
    #define htole32(x) bswap32(x)
    #define htole64(x) bswap64(x)
    #define le16toh(x) bswap16(x)
    #define le32toh(x) bswap32(x)
    #define le64toh(x) bswap64(x)
#endif

#endif // ndef _LIBC_SYS_ENDIAN_H
