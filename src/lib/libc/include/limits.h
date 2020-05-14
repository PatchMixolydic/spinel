#ifndef __LIBC_LIMITS_H
#define __LIBC_LIMITS_H

// Include compiler's architecture-dependent limits.h
#ifdef __GNUC__
#include_next <limits.h>
#else
#error "Your compiler doesn't support #include_next;\
 please file a bug report so that limits.h can be fixed"
#endif

// Quoth the POSIX:
// "A definition of one of the symbolic constants in the following list
// shall be omitted from the <limits.h> header on specific implementations
// where the corresponding value is equal to or greater than the stated
// minimum, but where the value can vary depending on the file to which it
// is applied.

// This varies by filesystem, and since it's the number of bits needed to store
// filesize as a signed integer and our off_t is uint64_t, this isn't very
// useful (how many applications support #define FILESIZEBITS 128?)
#undef FILESIZEBITS

#define LOGIN_NAME_MAX 512
#define SYMLOOP_MAX 32

// nlink_t == uint64_t
#define LINK_MAX UINT64_MAX
// Size of a filename, minus the nul
#define NAME_MAX 255
// Nonstandard: Size of a filename, incl. nul
#define NAME_BUFFER_SIZE (NAME_MAX + 1)
// Name of a tty device, including nul
#define TTY_NAME_MAX NAME_BUFFER_SIZE
#define PATH_MAX 4096
#define SYMLINK_MAX PATH_MAX


#endif // ndef __LIBC_LIMITS_H
