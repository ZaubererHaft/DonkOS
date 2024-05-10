#include "Process.h"
#include "DonkosInternal.h"

Process::Process() : pid{0U}, stack{0U} {}

void Process::Main() {
}

void Process::SetPid(uint32_t newPid) {
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
    //this currently results in an hard fault since we cannot directly request scheduling in unprivileged mode!
    auto lr = (uint32_t) Donkos_RequestScheduling;

    stack[stackSize - 1] = xpsr;
    stack[stackSize - 2] = pc;
    stack[stackSize - 3] = lr;

    return initialSp;
}

