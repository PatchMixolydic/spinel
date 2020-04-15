#ifndef SPINEL_RANDOM_H
#define SPINEL_RANDOM_H

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

void initRandom(void);
ssize_t readRandom(uint8_t* buf, size_t length);
uint32_t random32(void);
uint64_t random64(void);

#endif //ndef  SPINEL_RANDOM_H
