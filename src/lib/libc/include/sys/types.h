#ifndef _LIBC_SYS_TYPES_H
#define _LIBC_SYS_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// POSIX data types, and maybe more

// Filesizes
// 4 EiB ought to be enough for anyone :P
typedef uint64_t off_t;
// Likewise, 4 million users ought to be enough for anyone
typedef uint32_t uid_t;
// Group ID
typedef uint32_t gid_t;
// You will probably run out of memory before running 4 million processes
// But just in case...
typedef uint64_t pid_t;
// id_t must be able to hold at least a uid, gid, or a pid.
typedef uint64_t id_t;
// Number of symlinks/hard links
typedef uint64_t nlink_t;
// File permissions
typedef uint16_t mode_t;
// inode
typedef uint64_t ino_t;

// Time in nanoseconds
typedef int64_t suseconds_t;
// Time in seconds
typedef int64_t time_t;

// Signed size type
typedef int64_t ssize_t;

#endif // ndef _LIBC_SYS_TYPES_H
