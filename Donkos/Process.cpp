#include "Process.h"

Process::Process(uint32_t pid) : pid(pid) {}

void Process::Execute() {
}

uint32_t Process::GetPid() const {
    return pid;
}

