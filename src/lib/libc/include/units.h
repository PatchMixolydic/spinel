#ifndef __LIBC_UNITS_H
#define __LIBC_UNITS_H

// A hopefully helpful collection of units

// Bytes, binary
#define KiB 1024
#define MiB (1024 * KiB)
#define GiB (1024 * MiB)
#define TiB (1024 * GiB)
#define PiB (1024 * TiB)
#define EiB (1024 * PiB)
#define ZiB (1024 * EiB)
#define YiB (1024 * ZiB)
// Bytes, decimal
#define KB 1000
#define MB (1000 * KB)
#define GB (1000 * MB)
#define TB (1000 * GB)
#define PB (1000 * TB)
#define EB (1000 * PB)
#define ZB (1000 * EB)
#define YB (1000 * ZB)

// Time, in terms of seconds
#define Minute 60
#define Hour 3600
#define Day 86400

#endif // ndef __LIBC_UNITS_H
