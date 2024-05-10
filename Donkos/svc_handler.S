.global svc_handler
.global SVC_Handler_C
.syntax unified

/*
    Extracts the SVC handler id and passes it to a C function
*/
svc_handler:
    TST LR, #4
    ITE EQ
    MRSEQ R0, MSP
    MRSNE R0, PSP
    B SVC_Handler_C

.ALIGN 4
