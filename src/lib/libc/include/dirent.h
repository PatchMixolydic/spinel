#ifndef _LIBC_DIRENT_H
#define _LIBC_DIRENT_H

#include <sys/types.h>

struct dirent {
    // These two fields are required by POSIX
    ino_t d_ino;
    char d_name[256]; // TODO: constant
};

// This type is operating system dependent,
// so please don't use its innards in user space
typedef struct {
    ino_t vfsINode;
    size_t entryIdx;
    struct dirent* currentDirent;
} DIR;

#endif // ndef _LIBC_DIRENT_H
