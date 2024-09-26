#ifndef TEST_PRIORITYSCHEDULER_H
#define TEST_PRIORITYSCHEDULER_H

#include "BaseScheduler.h"
#include "BinaryHeap.h"

class PriorityScheduler : public BaseScheduler {
public:
    PriorityScheduler();

    void SetInitialProcess(Process *process) override;

    void RegisterProcess(Process *process) override;

    void Schedule() override;

    bool NeedsContextSwitch() const override;

    Process *GetCurrentProcess() override;

    void ContextSwitch(uint32_t *savedRegs) override;

    void UnregisterProcess(Process *process) override;

    void Tick() override;

private:
    BinaryHeap<Process *, 15> priortityQueue;
    uint32_t pidCounter;
    Process *current;
    Process *next;
};


#endif //TEST_PRIORITYSCHEDULER_H
