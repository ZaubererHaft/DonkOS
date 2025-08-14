#ifndef TEST_BASESCHEDULER_H
#define TEST_BASESCHEDULER_H

#include "Process.h"

class BaseScheduler {
public:
    virtual void SetInitialProcess(Process *process) = 0;

    virtual void RegisterProcess(Process *process) = 0;

    virtual void Schedule() = 0;

    virtual bool NeedsContextSwitch() const = 0;

    virtual Process *GetCurrentProcess() = 0;

    virtual void ContextSwitch(uint32_t *savedRegs) = 0;

    virtual void UnregisterProcess(Process *process) = 0;

    virtual void Tick() = 0;
};

#endif //TEST_BASESCHEDULER_H
