.global store_context
.syntax unified

/*
    R0: uint32_t *
    ret: updated sp
*/
store_context:
    PUSH {R4-R11}

    // load R4-R11 from args (8 regs) \n"
    LDMIA R0, {R4-R11}
    // the current task uses the PSP as stack pointer. load it to R0\n"
    MRS R0, PSP
    // push R4-R11 to stack of current process\n"
    STMDB R0!, {R4-R11}

    POP {R4-R11}
    BX LR
