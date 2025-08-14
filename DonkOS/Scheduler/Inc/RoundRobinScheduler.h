
#ifndef TEST_ROUNDROBINSCHEDULER_H
#define TEST_ROUNDROBINSCHEDULER_H

#include "Process.h"
#include "DonkosSettings.h"
#include "BaseScheduler.h"

class RoundRobinScheduler : public BaseScheduler {
public:
    RoundRobinScheduler();

    void SetInitialProcess(Process *process) override;

    void RegisterProcess(Process *process) override;

    void Schedule() override;

    bool NeedsContextSwitch() const override;

    Process * GetCurrentProcess() override;

    void ContextSwitch(uint32_t * savedRegs) override;

    void UnregisterProcess(Process *process) override;

    void Tick() override;

private:
    Process *processes[Donkos_MaxProcesses];
    uint32_t index;
    Process * currentProcess;
    Process * nextProcess;
};


#endif //TEST_ROUNDROBINSCHEDULER_H
