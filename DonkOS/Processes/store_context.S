.global store_context
.syntax unified

/*
    R0: uint32_t * (CONTROL,LR,R4-R11)
    ret: updated sp
*/
store_context:
    PUSH {R4-R11}

    // load R2-R11 from args (10 regs) \n"
    LDMIA R0, {R2-R11}

    // the current task uses the PSP as stack pointer. load it to R0\n"
    MRS R0, PSP

    TST R3, #0x10
    BNE .SKIP_FPS
    VSTMDB R0!, {S16-S31}
.SKIP_FPS:

    // push R2-R11 to stack of current process\n"
    STMDB R0!, {R2-R11}

    POP {R4-R11}

    BX LR
