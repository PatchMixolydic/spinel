#ifndef _LIBC_DIRENT_H
#define _LIBC_DIRENT_H

#include <limits.h>
#include <stdint.h>
#include <sys/types.h>

struct dirent {
    // These two fields are required by POSIX
    ino_t d_ino;
    char d_name[NAME_BUFFER_SIZE];
    // Offset from the beginning of the directory
    // to the next dirent, or 0 if this is the last
    // one
    off_t d_offsetToNext;
};

// This type is operating system dependent,
// so please don't use its innards in user space
typedef struct {
    struct dirent currentDirent;
    ino_t vfsINode;
    size_t entryIdx;
} DIR;

#endif // ndef _LIBC_DIRENT_H
