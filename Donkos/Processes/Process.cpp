#include "Process.h"
#include "DonkosInternal.h"

extern "C" void load_context(uint32_t, uint32_t *);
extern "C" uint32_t store_context(uint32_t *);

Process::Process() : pid{0U}, stack{0U}, stackPointer{0U}, state{ProcessState::CREATED} {}

void Process::Main() {
}

void Process::SetPid(uint32_t newPid) {
    Process::pid = newPid;
}


uint32_t Process::GetPid() const {
    return pid;
}

uint32_t Process::InitStack() {
    //stack grows downwards
    uint32_t stack_first_address = (uint32_t) stack + stackSize * 4;

    //set initial SP to last address
    //space for 16 regs? or why 16?
    auto initialSp = stack_first_address - 16 * 4;
    //Initial PC
    auto pc = (uint32_t) Donkos_GenericProcessMain;
    //Initial XPSR
    uint32_t xpsr = 0x01000000;

    auto lr = (uint32_t) 0;

    stack[stackSize - 1] = xpsr;
    stack[stackSize - 2] = pc;
    stack[stackSize - 3] = lr;

    stackPointer = initialSp;

    return initialSp;
}

void Process::SetState(ProcessState newState) {
    state = newState;
}

void Process::SaveContext(uint32_t *regs) {
    //saves the context stored in regs array of this process to the process' stack
    stackPointer = store_context(regs);
}

void Process::LoadContext(uint32_t *regs) {
    //loads the context of this process to the regs array
    load_context(stackPointer, regs);
}

uint32_t Process::GetStackPointer() const {
    return stackPointer;
}

ProcessState Process::GetState() const {
    return state;
}

void Process::UpdateTimer() {
    if (timer > 0) {
        timer--;

        if (timer <= 0) {
            if (state == ProcessState::WAITING) {
                state = ProcessState::READY;
            }
            timer = -1;
        }

    }
}

/*
 * waits asynchronously (at least) n milliseconds until the process continues
 * This will set the process to waiting state until the timer elapses
 */
void Process::wait(int32_t milliseconds) {
    timer = milliseconds;
    state = ProcessState::WAITING;
    Donkos_BlockProcess(this);
}

