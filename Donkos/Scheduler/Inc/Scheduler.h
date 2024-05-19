
#ifndef TEST_SCHEDULER_H
#define TEST_SCHEDULER_H

#include "Process.h"
#include "DonkosSettings.h"

class Scheduler {
public:
    Scheduler();

    void SetInitialProcess(Process *process);

    void RegisterProcess(Process *process);

    void Schedule();

    bool NeedsContextSwitch() const;

    Process * GetCurrentProcess();

    void ContextSwitch(uint32_t * savedRegs);

    void UnregisterProcess(Process *process);

    void Tick();

private:
    Process *processes[Donkos_MaxProcesses];
    uint32_t index;
    Process * currentProcess;
    Process * nextProcess;
};


#endif //TEST_SCHEDULER_H
