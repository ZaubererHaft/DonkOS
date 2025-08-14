.global load_context
.syntax unified

/*
    R0: uint32 sp of process;
    R1: uint32_t * space for CONTROL,LR,R4-R11;
*/
load_context:
    //push callee-saved regs to MSP
    PUSH {R4-R11}

    // load process' stack frame into R2-R11
    LDMIA R0!, {R2-R11}

    TST R3, #0x10
    BNE .SKIP_FPS
    VLDMIA R0!, {S16-S31}
.SKIP_FPS:

    // store regs in register array (argument)
    STMIA R1, {R2-R11}

    // update PSP to stack pointer of this process\n"
    MSR PSP, R0

    //pop callee-saved regs from MSP
    POP {R4-R11}

    BX LR
