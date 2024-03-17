.global context_switch
.syntax unified

context_switch:
    /*Save context*/
    MRS   R0, PSP
    STMDB R0!, {R4-R11}
    LDR   R1, =curr_task
    LDR   R2, [R1]
    LDR   R3, =PSP_array
    STR R0, [R3, R2, LSL #2]

    /*Load context*/
    LDR   R4, =next_task
    LDR   R4, [R4]
    STR   R4, [R1]
    LDR   R0, [R3, R4, LSL #2]
    LDMIA R0!, {R4-R11}

    MSR   PSP, R0
    BX LR
    .ALIGN 4
