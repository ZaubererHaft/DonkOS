#include "main.h"
#include "DonkosInternal.h"

extern "C" void ContextSwitch();

extern "C" void SVC_Handler_C(uint32_t *);

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
    //call the svc handler directly from assembly code
    __asm( ".global SVC_Handler_C\n"
           "TST lr, #4\n"
           "ITE EQ\n"
           "MRSEQ r0, MSP\n"
           "MRSNE r0, PSP\n"
           "B SVC_Handler_C\n"
            );

}

void SVC_Handler_C(uint32_t * args)
{
    __disable_irq();
    uint32_t svcNumber = ((char *) args[6])[-2];
    auto process = reinterpret_cast<Process *>(args[0]);
    Donkos_ServiceHandler(svcNumber, process);
    __enable_irq();
}


uint32_t regArray[10];
void ContextSwitch() {
    Donkos_ContextSwitch(regArray);
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
    __asm(
        // Here we call the context switch
        // First step: provide array for registers of the current running process (which however got interrupted now)
        // since R4-R11 and LR and CONTROL might be changed because of subroutines we need to save them immediately
            "LDR R1, =regArray\n"
            "ADD R1, #40\n"
            "STMDB R1!, {R4-R11};\n"
            "SUB R1, #4\n"
            "STR LR, [R1]\n"
            "MRS R0, CONTROL\n"
            "SUB R1, #4\n"
            "STR R0, [R1]\n"
            //now call context switch: this will save the array in the current process stack
            //and load the new variable into the provded array
            "BL ContextSwitch \n"
            //now finally apply the context switch by loading the R4-R11 and LR and CONTROL  from the array
            "LDR R1, =regArray\n"
            "MSR CONTROL, R1 \n"
            "ADD R1, #4\n"
            "LDR LR, [R1]\n"
            "ISB\n"
            "ADD R1, #4\n"
            "LDMIA R1!, {R4-R11};\n"
            "BX LR\n" //this should not be necessary
            );
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
    HAL_IncTick();
    Donkos_Tick();
    if (HAL_GetTick() % 1 == 0) {
        Donkos_RequestScheduling();
    }
}
