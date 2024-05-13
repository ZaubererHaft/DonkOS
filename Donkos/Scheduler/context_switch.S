.global context_switch
.syntax unified

/*
    The context switch is called from an interrupt handler
    Thus, we are using MSP for push / pop and are writing into the initial SP area.
    void context_switch(uint32_t *stackPointers, uint32_t pidCurr, uint32_t pidNext);
    R0 : uint32_t * stackPointers
    R1 : uint32_t pidCurr
    R2 : uint32_t pidNext
    R3 : uint32_t * regs
*/
context_switch:
    // load regs from argument
    LDMIA R3, {R4-R11}

    // the current task uses the PSP as stack pointer. load it to R3
    MRS R3, PSP

    // push R4-R11 to stack of current process
    STMDB R3!, {R4-R11}

    // stores updated stack pointer in the stackPointers array: stackPointers[pidCurr] = R3
    STR R3, [R0, R1, LSL #2]

    // R3 = stackPointers[pidNext]
    LDR R3, [R0, R2, LSL #2]

    // load R4-R11 from process stack (8 regs) to stack
    LDMIA R3!, {R4-R11}

    // update PSP to next stack pointer
    MSR PSP, R3

BX LR
.ALIGN 4
