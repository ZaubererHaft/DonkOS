#ifndef DONKOS_LOCK_H
#define DONKOS_LOCK_H

#include <cstdint>

class SimpleLock
{
public:
    SimpleLock();

    void Lock();

    void Unlock();

private:
    uint32_t lockObject;

    static void doLock(volatile uint32_t *lock);

    static void doUnlock(volatile uint32_t *lock);
};


#endif //DONKOS_LOCK_H
