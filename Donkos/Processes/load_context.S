.global load_context
.syntax unified

/*
    R0: uint32 sp;
    R1: uint32_t * regs;
*/
load_context:
    PUSH {R4-R11}

    // load R4-R11 from process stack (8 regs)\n"
    LDMIA R0!, {R4-R11}

    // store regs in register array
    STMIA R1, {R4-R11}

    // update PSP to next stack pointer\n"
    MSR PSP, R0

    POP {R4-R11}

    BX LR
