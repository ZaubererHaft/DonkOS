#include "Donkos.h"
#include "DonkosInternal.h"
#include "Process7SegmentDisplay.h"
#include "ProcessLed2.h"
#include "ProcessLed1.h"
#include "RoundRobinScheduler.h"
#include "ProcessMatrixDisplay.h"
#include "ADC3Process.h"
#include "DHTProcess.h"
#include "SSD1306Process.h"
#include "LedDisplay.h"
#include "BuzzerProcess.h"

extern "C" void ContextSwitch();

extern "C" void SVC_Handler_C(uint32_t *);

static void MX_GPIO_Init();

static void SystemClock_Config();

namespace {
    Display dotMatrix{};
    LedDisplay ledDisplay{};

    Process7SegmentDisplay mutexProcess{};
    ProcessLed1 led1Process{};
    ProcessLed2 led2Process{};
    ProcessMatrixDisplay pmd{&dotMatrix};
    ADC3Process temp{};
    //DHTProcess dht{};
    //SSD1306Process ssd1306Process{};
    BuzzerProcess buzz{};

    RoundRobinScheduler scheduler{};
}

void Donkos_MainLoop() {
    SCB->CCR |= SCB_CCR_STKALIGN_Msk;

    scheduler.RegisterProcess(&mutexProcess);
    scheduler.RegisterProcess(&led1Process);
    scheduler.RegisterProcess(&led2Process);
    scheduler.RegisterProcess(&buzz);
    scheduler.RegisterProcess(&pmd);
    scheduler.RegisterProcess(&temp);

    scheduler.SetInitialProcess(&buzz);

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

    temp.InitADC();

    //Digital
    MX_GPIO_Init();

    dotMatrix.Initialize();
    ledDisplay.Init();
    buzz.Init();
}

RoundRobinScheduler &Donkos_GetScheduler() {
    return scheduler;
}

void Donkos_RequestScheduling() {
    scheduler.Schedule();
    if (scheduler.NeedsContextSwitch()) {
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    }
}

void Donkos_StartNewProcess(Process *process) {
    asm("SVC #0x2;");
    //wait until task is registered to the scheduler
    //with that we should make sure that the argument on the stack is still valid
    //but not 100 % sure yet
    while (process->GetState() == ProcessState::CREATED) {
        __NOP();
    }
}

void Donkos_YieldProcess(Process *process) {
    //reschedule only. Here, a process can manually request a context switch
    asm("SVC #0x1;");
}

void Donkos_BlockProcess(Process *process) {
    asm("SVC #0x1;");
    // now wait until interrupt has been served
    // process will not be scheduled again until the resource the process was waiting for was freed
    // it will then continue executing after the while loop
    while (process->GetState() == ProcessState::WAITING) {
        __NOP();
    }
}

void Donkos_EndProcess(Process *process) {
    //unregister and request scheduling again
    asm("SVC #0x0;");
    // wait for process to end
    while (true) {
        __NOP();
    }
}


void Donkos_Display(const char *text) {
    ledDisplay.Display(text);
}

void Donkos_SetDisplayLine(uint32_t line) {
    ledDisplay.SetLine(line);
}


void Donkos_GenericProcessMain() {
    Process *p = scheduler.GetCurrentProcess();
    p->Main();
    Donkos_EndProcess(p);
}

void SysTick_Handler(void) {
    HAL_IncTick();
    scheduler.Tick();
    if (HAL_GetTick() % 1 == 0) {
        Donkos_RequestScheduling();
    }
}

uint32_t regArray[10];

void ContextSwitch() {
    scheduler.ContextSwitch(regArray);
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void) {
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
    __disable_irq();

    uint32_t svcNumber = ((char *) args[6])[-2];

    //unregister & reschedule
    if (svcNumber == 0) {
        //since PENDSV has a lower prio than SVC it would safe to unregister without IRQ disabling
        // however since scheduling request is privileged -> SVC call
        auto process = reinterpret_cast<Process *>(args[0]);
        scheduler.UnregisterProcess(process);
        Donkos_RequestScheduling();
    }
    //reschedule only
    else if (svcNumber == 1) {
        Donkos_RequestScheduling();
    //start a new process
    } else if (svcNumber == 2) {
        auto process = reinterpret_cast<Process *>(args[0]);
        scheduler.RegisterProcess(process);
    }
    __enable_irq();
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
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
    RCC_OscInitStruct.PLL.PLLM = 1;
    RCC_OscInitStruct.PLL.PLLN = 30;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                  | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK) {
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
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, SHCP_Pin|STCP_Pin|DSSER_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, LED_1_Pin|LED2_Pin|DISPLAY_CS_Pin|DHT_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin : BUTTON_Pin */
    GPIO_InitStruct.Pin = BUTTON_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BUTTON_GPIO_Port, &GPIO_InitStruct);

    /*Configure GPIO pins : SHCP_Pin STCP_Pin DSSER_Pin */
    GPIO_InitStruct.Pin = SHCP_Pin|STCP_Pin|DSSER_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : LED_1_Pin LED2_Pin DISPLAY_CS_Pin DHT_Pin */
    GPIO_InitStruct.Pin = LED_1_Pin|LED2_Pin|DISPLAY_CS_Pin|DHT_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
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

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    static bool show = false;
    ledDisplay.SetLine(2);

    show = !show;
    Donkos_SetDisplayLine(2);
    if(show)
    {
        Donkos_Display("Hi!");
    }
    else
    {
        Donkos_Display("            ");
    }
}
