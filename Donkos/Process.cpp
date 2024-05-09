#include "Process.h"
#include "DonkosInternal.h"

Process::Process() : pid(0U), stack{0U} {}

void Process::Execute() {
}

void Process::SetPid(uint32_t pid) {
    Process::pid = pid;
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

    stack[stackSize - 1] = xpsr;
    stack[stackSize - 2] = pc;
    return initialSp;
}

