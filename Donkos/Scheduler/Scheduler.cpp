#include "Scheduler.h"

extern "C" void context_switch(uint32_t *stackPointers, uint32_t pidCurr, uint32_t pidNext);

Scheduler::Scheduler() : processes{nullptr}, index{0U}, currentProcess{0U}, nextProcess{0U} {

}

void Scheduler::RegisterProcess(Process *process) {
    processes[index] = process;
    index++;
}

void Scheduler::Schedule() {
    if (index > 0) {
        nextProcess = (currentProcess + 1) % index;
    }
}

bool Scheduler::NeedsContextSwitch() {
    if (index <= 0) {
        return false;
    } else {
        return nextProcess != currentProcess;
    }
}


Process *Scheduler::GetNextProcess() {
    return processes[currentProcess];
}

Process *Scheduler::GetCurrentProcess() {
    return processes[nextProcess];
}

void Scheduler::ContextSwitch(uint32_t *pspArray) {
    auto curr = currentProcess;
    auto next = nextProcess;
    currentProcess = next;

    //after context switch, currentProcess = nextProcess, so no scheduling needed, as long as schedule() has not been called
    context_switch(pspArray, curr, next);
}
