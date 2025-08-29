#include "Lock.h"
#include "main.h"

SimpleLock::SimpleLock() : lockObject{} {

}


void SimpleLock::Lock() {
    doLock(&lockObject);
}

void SimpleLock::Unlock() {
    doUnlock(&lockObject);
}

void SimpleLock::doLock(volatile uint32_t *lock) {

    while (1) {
        if (__LDREXW(lock) == 0) {
            if (__STREXW(1, lock) == 0) {
                __DMB(); // Data Memory Barrier
                return;
            }
        }
        __CLREX(); // Clear exclusive state if STREX failed
    }
}


void SimpleLock::doUnlock(volatile uint32_t *lock) {
    __DMB();     // Ensure memory operations complete before releasing
    *lock = 0;
}
