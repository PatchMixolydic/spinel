#include <spinel/concurrency.h>

void spinlockMutex(Mutex* mutex) {
    // This will return true when its previous value was true
    // If its previous value was false, we locked it
    while (atomic_flag_test_and_set(mutex)) {}
}

void unlockMutex(Mutex* mutex) {
    atomic_flag_clear(mutex);
}
