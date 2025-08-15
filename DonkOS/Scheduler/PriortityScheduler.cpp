#include "PriorityScheduler.h"
#include "main.h"

PriorityScheduler::PriorityScheduler() : priortityQueue{}, pidCounter{0}, current{nullptr}, next{nullptr} {
}

void PriorityScheduler::SetInitialProcess(Process *process) {
    __set_PSP(process->GetStackPointer());
    priortityQueue.Delete(process);
    current = process;
    next = process;
}

void PriorityScheduler::RegisterProcess(Process *process) {
    if (priortityQueue.Insert(process)) {
        pidCounter++;
        process->SetState(ProcessState::READY);
        process->SetPid(pidCounter);
        process->InitStack();
    }
}

void PriorityScheduler::Schedule() {
    Process *process;
    if (priortityQueue.PopMax(process)) {
        next = process;
    }
}

bool PriorityScheduler::NeedsContextSwitch() const {
    return next != current;
}

Process *PriorityScheduler::GetCurrentProcess() {
    return current;
}

void PriorityScheduler::ContextSwitch(uint32_t *savedRegs) {
    if (current != nullptr) {
        current->SaveContext(savedRegs);

        //if process ended up waiting do not override its state s.t. it won't get scheduled until it is not waiting anymore
        // -> set only ready if process is running
        if (current->GetState() == ProcessState::RUNNING) {
            current->SetState(ProcessState::READY);
        }

        if(current->GetState() != ProcessState::ENDED)
        {
            // if process is not finished re-enter into queue
            priortityQueue.Insert(current);
        }
    }

    current = next;

    if (next != nullptr) {
        next->LoadContext(savedRegs);
        next->SetState(ProcessState::RUNNING);
    }
}

void PriorityScheduler::UnregisterProcess(Process *process) {
    priortityQueue.Delete(process);
}

void PriorityScheduler::Tick() {
    auto [tmp_Array, tmp_Len] = priortityQueue.DecayToArray();
    for (int32_t i = 0; i < tmp_Len; ++i) {
        tmp_Array[i]->UpdateTimer();
    }
}

