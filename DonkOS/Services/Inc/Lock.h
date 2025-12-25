#ifndef DONKOS_LOCK_H
#define DONKOS_LOCK_H

#include <cstdint>

class SimpleLock
{
public:
    SimpleLock();

    void SpinLock(uint16_t with_id);

    void YieldLock(uint16_t with_id);

    bool Lock(uint16_t with_id);

    void Unlock(uint16_t from_id);

private:
    uint32_t lockObject;
    int32_t owner;
};


#endif //DONKOS_LOCK_H
