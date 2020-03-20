#ifndef SPINEL_CONCURRENCY_H
#define SPINEL_CONCURRENCY_H

#ifdef __STDC_NO_ATOMICS__
// TODO: our own stdatomic.h
#error "This compiler does not have freestanding stdatomic.h"
#endif

#include <stdatomic.h>

typedef atomic_bool Mutex;
void spinlockMutex(Mutex* mutex);
void unlockMutex(Mutex* mutex);

#endif // ndef SPINEL_CONCURRENCY_H
