#include "Lock.h"
#include "main.h"
#include "DonkosInternal.h"

SimpleLock::SimpleLock() : lockObject{}, owner{nullptr}, wait_queue{} {
}

bool SimpleLock::AutoLock(Process *process) {
    bool ret = true;
    if (!Lock(process)) {
        if (wait_queue.Length() < wait_queue.Capacity()) {
            wait_queue.Push(process);

            //The current process is now frozen in this function call
            Donkos_BlockProcess(process);

        } else {
            ret = false;
        }
    }
    return ret;
}

bool SimpleLock::Lock(Process *process) {
    if (lockObject == 0) {
        if (__LDREXW(&lockObject) == 0) {
            if (__STREXW(1, &lockObject) == 0) {
                __DMB(); // Ensure lock is seen before writing owner
                owner = process;
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


void SimpleLock::Unlock(const Process *process) {
    if (process == owner) {
        __DMB(); // Ensure memory operations complete before releasing
        owner = nullptr;
        lockObject = 0;
        if (wait_queue.Length() > 0) {
            auto *process_to_wakeup = wait_queue.Pop();
            Donkos_WakeUp(process_to_wakeup);
        }
        __DMB();
    }
}
