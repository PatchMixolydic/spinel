#ifndef _LIBC_DIRENT_H
#define _LIBC_DIRENT_H

#include <sys/types.h>

struct dirent {
    // These two fields are required by POSIX
    ino_t d_ino;
    char d_name[256]; // TODO: constant
};

#endif // ndef _LIBC_DIRENT_H
