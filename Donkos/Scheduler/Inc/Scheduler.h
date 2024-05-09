
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

    void ContextSwitch();

private:
    uint32_t PSP_array[Donkos_MaxProcesses];
    Process *processes[Donkos_MaxProcesses];
    uint32_t index;
    uint32_t currentProcess;
    uint32_t nextProcess;
};


#endif //TEST_SCHEDULER_H
