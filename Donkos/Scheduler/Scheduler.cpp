#include "Scheduler.h"

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
