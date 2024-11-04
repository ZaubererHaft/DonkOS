#include "Process.h"
#include "DonkosInternal.h"

extern "C" void load_context(uint32_t, uint32_t *);
extern "C" uint32_t store_context(uint32_t *);

Process::Process() : pid{0U}, heap{0U}, stack{0U}, stackPointer{0U}, state{ProcessState::CREATED},
                     heapAllocator{&heap[0], heapSizeInMultipleOf4}, priority{0U} {}

void Process::Main() {
}

void Process::SetPid(uint32_t newPid) {
    Process::pid = newPid;
}


uint32_t Process::GetPid() const {
    return pid;
}

uint32_t Process::InitStack() {
    //Note: stack grows downwards from high to low addresses
    //I.e., in C++, the top of the stack is the last position in the array

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

    uint32_t indexer = stackSizeInMultipleOf4 - 1;

    setReg(indexer, xpsr);
    setReg(indexer, pc);
    setReg(indexer, lr);
    setReg(indexer, R12);
    setReg(indexer, R3);
    setReg(indexer, R2);
    setReg(indexer, R1);
    setReg(indexer, R0);

    //initial callee register values; in a context switch, they get updated by the OS
    uint32_t R11 = 0;
    uint32_t R10 = 0;
    uint32_t R9 = 0;
    uint32_t R8 = 0;
    uint32_t R7 = 0;
    uint32_t R6 = 0;
    uint32_t R5 = 0;
    uint32_t R4 = 0;
    uint32_t exec_return = 0xFFFFFFFD;  //NO FPU
    uint32_t control = 0x3; // No FPU, use PSP, unprivileged

    setReg(indexer, R11);
    setReg(indexer, R10);
    setReg(indexer, R9);
    setReg(indexer, R8);
    setReg(indexer, R7);
    setReg(indexer, R6);
    setReg(indexer, R5);
    setReg(indexer, R4);
    setReg(indexer, exec_return);
    setReg(indexer, control);

    //set initial SP to last address
    auto initialSp = (uint32_t) &stack[stackSizeInMultipleOf4 - 18];
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

void Process::setReg(uint32_t &indexer, uint32_t reg) {
    stack[indexer] = reg;
    indexer--;
}

bool Process::StackPtrValid() {
    return stackPointer >= (uint32_t) &stack[0] && stackPointer < (uint32_t) &stack[stackSizeInMultipleOf4];
}

MemoryListAllocator &Process::GetHeapAllocator() {
    return heapAllocator;
}

uint8_t Process::GetPriority() const {
    return priority;
}

void Process::SetPriority(uint8_t prio) {
    priority = prio;
}

bool Process::operator<(Process *process) const {
    return priority < process->priority;
}

uint32_t Process::GetStackStartAddress() {
    return (uint32_t)&stack[0];
}

uint32_t Process::GetDataStartAddress() {
    return (uint32_t)&heap[0];
}

uint32_t Process::GetStackSizeInBytes() {
    return stackSizeInMultipleOf4 * sizeof(uint32_t);
}

uint32_t Process::GetDataSizeInBytes() {
    return heapSizeInMultipleOf4 * sizeof(uint32_t);
}
