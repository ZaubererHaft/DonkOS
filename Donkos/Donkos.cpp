#include "Donkos.h"
#include "DonkosInternal.h"
#include "Process7SegmentDisplay.h"
#include "ProcessLed2.h"
#include "ProcessLed1.h"
#include "Scheduler.h"
#include "ProcessNoLoop.h"
#include "ProcessMatrixDisplay.h"
#include "TemperatureProcess.h"

extern "C" void SVC_Handler_C(uint32_t *);

static void MX_GPIO_Init();

static void SystemClock_Config();

namespace {
    Display dotMatrix{};

    Process7SegmentDisplay mutexProcess{};
    ProcessLed1 led1Process{};
    ProcessLed2 led2Process{};
    ProcessNoLoop noloopProcess{};
    ProcessMatrixDisplay pmd{&dotMatrix};
    TemperatureProcess temp{};

    Scheduler scheduler{};
}

void Donkos_MainLoop() {
    SCB->CCR |= SCB_CCR_STKALIGN_Msk;

    Donkos_StartProcess(&mutexProcess);
    Donkos_StartProcess(&led1Process);
    Donkos_StartProcess(&led2Process);
    Donkos_StartProcess(&noloopProcess);
    Donkos_StartProcess(&pmd);
    Donkos_StartProcess(&temp);

    scheduler.SetInitialProcess(&mutexProcess);

    NVIC_SetPriority(PendSV_IRQn, 0xFF);

    __set_CONTROL(0x3); //unprivileged mode starting here
    __ISB();
    __WFI();

    while (true) {
        __NOP();
    }
}

void Donkos_Init() {
    //Driver init
    HAL_Init();

    //Clock Init
    SystemClock_Config();

    //Digital
    MX_GPIO_Init();
}

void Donkos_RequestScheduling() {
    scheduler.Schedule();
    if (scheduler.NeedsContextSwitch()) {
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    }
}

void Donkos_StartProcess(Process *process) {
    scheduler.RegisterProcess(process);
}

void Donkos_BlockProcess(Process *process) {
    //request scheduling again
    asm("SVC #0x1;");
    // now wait until interrupt has been served
    while (process->GetState() == ProcessState::WAITING) {
        __NOP();
    }
}

void Donkos_EndProcess(Process *process) {
    //request scheduling again
    asm("SVC #0x0;");
    // wait for process to end
    while (true) {
        __NOP();
    }
}

void Donkos_DisplayNumber(uint8_t number) {
    dotMatrix.Show(number);
}

void Donkos_GenericProcessMain() {
    Process *p = scheduler.GetCurrentProcess();
    p->Main();
    Donkos_EndProcess(p);
}

void SysTick_Handler(void) {
    HAL_IncTick();
    scheduler.Tick();
    Donkos_RequestScheduling();
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void) {
    uint32_t *tmpArr;
    // Here we call the context switch
    // First step: provide array for registers of the current running process (which however got interrupted now)
    // since R4-R11 might be changed because of local variables we need to save them immediately
    __asm("SUB SP, #32;\n"
          "STMIA SP, {R4-R11};\n"
          "MOV %[arr], SP;\n"
            : [arr] "=r"(tmpArr));

    //now call context switch: this will save the array in the current process stack
    //and load the new variable into the provded array
    scheduler.ContextSwitch(tmpArr);

    //now finally apply the context switch by loading the R4-R11 from the array
    __asm("LDMIA SP, {R4-R11};\n"
          "ADD SP, #32;\n");
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void) {
    //call the svc handler directly from assembly code
    __asm( ".global SVC_Handler_C\n"
           "TST lr, #4\n"
           "ITE EQ\n"
           "MRSEQ r0, MSP\n"
           "MRSNE r0, PSP\n"
           "B SVC_Handler_C\n"
            );
}

void SVC_Handler_C(uint32_t *args) {

    uint32_t svcNumber = ((char *) args[6])[-2];

    //unregister & reschedule
    if (svcNumber == 0) {
        //since PENDSV has a lower prio than SVC it would safe to unregister without IRQ disabling
        // however since scheduling request is privileged -> SVC call
        auto process = reinterpret_cast<Process *>(args[0]);
        __disable_irq();
        scheduler.UnregisterProcess(process);
        Donkos_RequestScheduling();
        __enable_irq();
    }
        //reschedule only
    else if (svcNumber == 1) {
        __disable_irq();
        Donkos_RequestScheduling();
        __enable_irq();
    }

}


/**
  * @brief System Clock Configuration
  * @retval None
  */
static void SystemClock_Config() {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Configure the main internal regulator output voltage
    */
    if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
    RCC_OscInitStruct.MSIState = RCC_MSI_ON;
    RCC_OscInitStruct.MSICalibrationValue = 0;
    RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK) {
        Error_Handler();
    }
}


/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, SHCP_Pin | STCP_Pin | DSSER_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, LED_1_Pin | LED2_Pin | DISPLAY_CS_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : SHCP_Pin STCP_Pin DSSER_Pin */
    GPIO_InitStruct.Pin = SHCP_Pin | STCP_Pin | DSSER_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : LED_1_Pin LED2_Pin DISPLAY_CS_Pin */
    GPIO_InitStruct.Pin = LED_1_Pin | LED2_Pin | DISPLAY_CS_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
}
