#include "Scheduler.h"
#include "stm32l4xx_hal.h"

extern "C" void context_switch(uint32_t *stackPointers, uint32_t pidCurr, uint32_t pidNext);

Scheduler::Scheduler() : PSP_array{0U}, processes{nullptr}, index{0U}, currentProcess{0U}, nextProcess{0U} {

}

void Scheduler::SetInitialProcess(Process *process) {
    __set_PSP(PSP_array[process->GetPid()]);
    currentProcess = process->GetPid() <= 0U ? index : process->GetPid() - 1U;
}


void Scheduler::RegisterProcess(Process *process) {
    if (index < Donkos_MaxProcesses) {
        process->SetPid(index);
        PSP_array[process->GetPid()] = process->InitStack();
        processes[process->GetPid()] = process;
        index++;
    }
}

void Scheduler::Schedule() {
    if (index > 0) {
        nextProcess = (currentProcess + 1) % index;
    }
}

bool Scheduler::NeedsContextSwitch() const {
    if (index <= 0) {
        return false;
    } else {
        return nextProcess != currentProcess;
    }
}

Process *Scheduler::GetCurrentProcess() {
    return processes[nextProcess];
}

void Scheduler::ContextSwitch() {
    auto curr = currentProcess;
    auto next = nextProcess;
    currentProcess = next;

    //after context switch, currentProcess = nextProcess, so no scheduling needed, as long as schedule() has not been called
    context_switch(&PSP_array[0], curr, next);
}

void Scheduler::UnregisterProcess(Process *process) {
    if (process->GetPid() < index) {
        for (uint32_t i = process->GetPid(); i < index - 1; ++i) {
            processes[i] = processes[i + 1];
            processes[i]->SetPid(i);
        }
        processes[index - 1] = nullptr;
        index--;
    }

}
