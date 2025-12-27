#ifndef DONKOS_LOCK_H
#define DONKOS_LOCK_H

#include <cstdint>

#include "Process.h"
#include "Queue.h"

class SimpleLock
{
public:
    SimpleLock();

    bool AutoLock(Process* process);

    bool Lock(Process* process);

    void Unlock(const Process* process);

private:
    static constexpr int32_t MAX_PROCESSES_TO_WAIT_FOR_LOCK = 10;
    uint32_t lockObject;
    Process* owner;

    Queue<Process *, MAX_PROCESSES_TO_WAIT_FOR_LOCK> wait_queue;
};


#endif //DONKOS_LOCK_H
