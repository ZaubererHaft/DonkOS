
#ifndef TEST_SCHEDULER_H
#define TEST_SCHEDULER_H

#include "Process.h"
#include "System.h"

class Scheduler {
public:
    Scheduler();

    void RegisterProcess(Process *process);

    void Schedule();

    bool NeedsContextSwitch();

    Process * GetNextProcess();

    Process * GetCurrentProcess();

    void ContextSwitch(uint32_t * pspArray);

private:
    Process *processes[MaxProcesses];
    uint32_t index;
    uint32_t currentProcess;
    uint32_t nextProcess;
};


#endif //TEST_SCHEDULER_H
