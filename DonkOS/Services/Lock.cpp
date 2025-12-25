#include "Lock.h"
#include "main.h"
#include "DonkosInternal.h"

SimpleLock::SimpleLock() : lockObject{}, owner{-1} {
}

void SimpleLock::SpinLock(uint16_t with_id) {
    while (!Lock(with_id)) {
    }
}

void SimpleLock::YieldLock(uint16_t with_id) {
    while (!Lock(with_id)) {
        Donkos_YieldCurrentProcess();
    }
}

bool SimpleLock::Lock(uint16_t with_id) {
    if (lockObject == 0) {
        if (__LDREXW(&lockObject) == 0) {
            if (__STREXW(1, &lockObject) == 0) {
                __DMB(); // Ensure lock is seen before writing owner
                owner = with_id;
                __DMB(); // Ensure owner is seen before entering critical section
                return true;
            }
        } else {
            // Decision point: If LDREX was successful but STREX was skipped
            // because lockObject was != 0, call __CLREX to clear the monitor.
            __CLREX();
        }
    }
    return false;
}


void SimpleLock::Unlock(uint16_t from_id) {
    if (from_id == owner) {
        __DMB(); // Ensure memory operations complete before releasing
        owner = -1;
        lockObject = 0;
        __DMB();
    }
}
