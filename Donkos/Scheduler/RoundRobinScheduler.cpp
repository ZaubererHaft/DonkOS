#include "RoundRobinScheduler.h"
#include "stm32l4xx_hal.h"

extern "C" void context_switch(uint32_t *stackPointers, uint32_t pidCurr, uint32_t pidNext, uint32_t *savedRegs);

RoundRobinScheduler::RoundRobinScheduler() : processes{nullptr}, index{0U}, currentProcess{nullptr}, nextProcess{nullptr} {

}

void RoundRobinScheduler::SetInitialProcess(Process *process) {
    __set_PSP(process->GetStackPointer());
    nextProcess = process;
}


void RoundRobinScheduler::RegisterProcess(Process *process) {
    if (index < Donkos_MaxProcesses) {
        process->SetState(ProcessState::READY);
        process->SetPid(index);
        process->InitStack();
        processes[process->GetPid()] = process;
        index++;
    }
}

void RoundRobinScheduler::Schedule() {
    if (index > 0) {
        if (currentProcess != nullptr) {
            uint32_t pidStart = currentProcess->GetPid();
            uint32_t iterations = 0;
            do {
                pidStart++;
                uint32_t nextPid = pidStart % index;
                nextProcess = processes[nextPid];
                iterations++;
                //ignore processes that wait for a resource and continue until all processes where iterated
            } while (nextProcess->GetState() == ProcessState::WAITING && iterations < index);

            // if no next non-waiting process found -> no context switch
            // in the worst case the system is idling with NOPs of the current process
            // idea for improvement: go to sleep mode
            if (iterations >= index) {
                nextProcess = currentProcess;
            }
        }
    }
}

bool RoundRobinScheduler::NeedsContextSwitch() const {
    if (index <= 0) {
        return false;
    } else {
        return nextProcess != currentProcess;
    }
}

Process *RoundRobinScheduler::GetCurrentProcess() {
    return currentProcess;
}

void RoundRobinScheduler::ContextSwitch(uint32_t *savedRegs) {
    if (currentProcess != nullptr) {
        currentProcess->SaveContext(savedRegs);

        //if process ended up waiting do not override its state s.t. it won't get scheduled until it is not waiting anymore
        if (currentProcess->GetState() == ProcessState::RUNNING) {
            currentProcess->SetState(ProcessState::READY);
        }
    }

    currentProcess = nextProcess;

    if (nextProcess != nullptr) {
        nextProcess->LoadContext(savedRegs);
        nextProcess->SetState(ProcessState::RUNNING);
    }
}

void RoundRobinScheduler::UnregisterProcess(Process *process) {
    // remove process from list
    if (process->GetPid() < index) {
        for (uint32_t i = process->GetPid(); i < index - 1; ++i) {
            processes[i] = processes[i + 1];
            processes[i]->SetPid(i);
        }
        processes[index - 1] = nullptr;
        index--;
    }
    process->SetState(ProcessState::CREATED);
}

void RoundRobinScheduler::Tick() {
    for (uint32_t i = 0; i < index; ++i) {
        processes[i]->UpdateTimer();
    }
}
