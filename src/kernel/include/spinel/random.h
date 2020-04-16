#ifndef SPINEL_RANDOM_H
#define SPINEL_RANDOM_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

void initRandom(void);
// Read a stream of random bytes from the RNG
// May return less than the requested length
// Returns -EFAULT on bad buf or number of bytes read otherwise
ssize_t readRandom(uint8_t* buf, size_t length);
// Use bytes as entropy
// Returns -EFAULT on bad buf or length
ssize_t feedRandom(uint8_t* buf, size_t length);
// Get a random 32 bit number
uint32_t random32(void);
// Get a uniformly-distributed random 32 bit number
uint32_t randomRange32(uint32_t rangeStart, uint32_t rangeEnd);
uint64_t random64(void);

#endif //ndef  SPINEL_RANDOM_H
