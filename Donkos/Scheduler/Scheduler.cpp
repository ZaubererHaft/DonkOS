#include "Scheduler.h"
#include "stm32l4xx_hal.h"

extern "C" void context_switch(uint32_t *stackPointers, uint32_t pidCurr, uint32_t pidNext, uint32_t *savedRegs);

Scheduler::Scheduler() : processes{nullptr}, index{0U}, currentProcess{nullptr}, nextProcess{nullptr} {

}

void Scheduler::SetInitialProcess(Process *process) {
    __set_PSP(process->GetStackPointer());
    nextProcess = process;
}


void Scheduler::RegisterProcess(Process *process) {
    if (index < Donkos_MaxProcesses) {
        process->SetPid(index);
        process->InitStack();
        processes[process->GetPid()] = process;
        index++;
    }
}

void Scheduler::Schedule() {
    if (index > 0) {

        if (currentProcess != nullptr) {
            uint32_t nextPid = (currentProcess->GetPid() + 1) % index;
            nextProcess = processes[nextPid];
        }
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
    return currentProcess;
}

void Scheduler::ContextSwitch(uint32_t *savedRegs) {
    if (currentProcess != nullptr) {
        currentProcess->SaveContext(savedRegs);
    }

    currentProcess = nextProcess;

    if (nextProcess != nullptr) {
        nextProcess->LoadContext(savedRegs);
    }
}

void Scheduler::UnregisterProcess(Process *process) {
    // remove process from list
    if (process->GetPid() < index) {
        for (uint32_t i = process->GetPid(); i < index - 1; ++i) {
            processes[i] = processes[i + 1];
            processes[i]->SetPid(i);
        }
        processes[index - 1] = nullptr;
        index--;
    }
}
