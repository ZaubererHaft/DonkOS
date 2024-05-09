.global context_switch
.syntax unified

/*
    The context switch is called from an interrupt handler
    Thus, we are using MSP for push / pop and are writing into the initial SP area.
*/
context_switch:
    // the current task uses the PSP as stack pointer. load it to R0
    MRS R0, PSP
    // R4-R11 are callee saved, thus they need to be saved when performing a context switch
    // the instruction "pushes" R4-R11 to the PSP: SUB R0, #4 | STR R11, [R0] | SUB R0, #4 | STR R10, [R0], ...
    STMDB R0!, {R4-R11}

    LDR R1, =curr_task //R1 = &curr_task
    LDR R2, [R1]       //R2 = *curr_task
    LDR R3, =PSP_array //R3 = &PSP_array[0]

    // Store R0 to R3 + (R2 << n) = R3 + R2 * 2^n
    // translates to: *(R2 + curr_task * 4) = R0
    // or more easily: PSP_array[*curr_task] = R0
    // stores the stack pointer in the PSP array
    STR R0, [R3, R2, LSL #2]

    /*Load context*/
    LDR   R4, =next_task //R4 = &next_task
    LDR   R4, [R4]       //R4 = *next_task
    STR   R4, [R1]       //*curr_task = next_task

    LDR   R0, [R3, R4, LSL #2] // R0 = R3 + (R4 << n) = R3 + R4 * 2^n = PSP_array[*curr_task]
    LDMIA R0!, {R4-R11}  //load R4-R11 from process stack (8 regs)

    MSR   PSP, R0 // update PSP
    BX LR

    .ALIGN 4
