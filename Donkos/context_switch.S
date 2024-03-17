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
    LDR   R4, [R4] /*next task id*/
    STR   R4, [R1] /*curr_task = next_task*/
    LDR   R0, [R3, R4, LSL #2] /*load PSP value*/
    LDMIA R0!, {R4-R11} /*load R4-R11 from stack (8 regs)*/

    MSR   PSP, R0 /*set PSP to next*/
    BX LR
    .ALIGN 4
