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
    uint32_t stack_first_address = (uint32_t) stack + stackSizeInByte * sizeof(uint32_t);

    //initial caller register values; in a context switch, they get updated by HW
    //this is just a reminder how it works; will be optimized anyway
    uint32_t xpsr = 0x01000000;
    uint32_t pc = (uint32_t) Donkos_GenericProcessMain;
    uint32_t lr = 0;
    uint32_t R12 = 0;
    uint32_t R3 = 0;
    uint32_t R2 = 0;
    uint32_t R1 = 0;
    uint32_t R0 = 0;

    //initial callee register values; in a context switch, they get updated by the OS
    uint32_t R11 = 0;
    uint32_t R10 = 0;
    uint32_t R9 = 0;
    uint32_t R8 = 0;
    uint32_t R7 = 0;
    uint32_t R6 = 0;
    uint32_t R5 = 0;
    uint32_t R4 = 0;
    uint32_t exec_return = 0xFFFFFFFD;
    uint32_t control = 0x3;

    stack[stackSizeInByte - 1] = xpsr;
    stack[stackSizeInByte - 2] = pc;
    stack[stackSizeInByte - 3] = lr;
    stack[stackSizeInByte - 4] = R12;
    stack[stackSizeInByte - 5] = R3;
    stack[stackSizeInByte - 6] = R2;
    stack[stackSizeInByte - 7] = R1;
    stack[stackSizeInByte - 8] = R0;

    stack[stackSizeInByte - 9] = R11;
    stack[stackSizeInByte - 10] = R10;
    stack[stackSizeInByte - 11] = R9;
    stack[stackSizeInByte - 12] = R8;
    stack[stackSizeInByte - 13] = R7;
    stack[stackSizeInByte - 14] = R6;
    stack[stackSizeInByte - 15] = R5;
    stack[stackSizeInByte - 16] = R4;

    stack[stackSizeInByte - 17] = exec_return;
    stack[stackSizeInByte - 18] = control;

    //set initial SP to last address
    //space for 16 regs? or why 16?
    auto initialSp = (uint32_t) &stack[stackSizeInByte - 18];
    stackPointer = initialSp;
    return stackPointer;
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

