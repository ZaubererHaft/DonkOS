#include "UserTask.h"
#include "stm32l4xx_hal_adc.h"
#include "stm32l4xx_it.h"

extern void context_switch();

static void MX_ADC1_Init();

static void MX_GPIO_Init();

static void onButtonPressed(uint8_t i);

static uint16_t readAnalogIn();

static void SystemClock_Config();

static ADC_HandleTypeDef hadc1;
static GPIO_PinState ledState = GPIO_PIN_RESET;

void task0(void);

void task1(void);

void task2(void);

void task3(void);

uint32_t task0_stack[1024];
uint32_t task1_stack[1024];
uint32_t task2_stack[1024];
uint32_t task3_stack[1024];

uint32_t curr_task = 0;
uint32_t next_task = 1;
uint32_t PSP_array[4];

void InitStack(uint32_t task_id, void (*task_main)(void), uint32_t *stack, uint32_t stack_size) {
    //stack grows downwards
    uint32_t stack_first_address = (uint32_t) stack + stack_size * 4;

    //set initial SP to last address, 16 registers available
    PSP_array[task_id] = stack_first_address - 16 * 4;
    //Initial PC
    uint32_t pc = (uint32_t) task_main;
    //Initial XPSR
    uint32_t xpsr = 0x01000000;

    stack[stack_size - 15] = pc;
    stack[stack_size - 16] = xpsr;
}

void Donkos_MainLoop() {
    InitStack(0, &task0, &task0_stack[0], 1024);
    InitStack(1, &task1, &task1_stack[0], 1024);
    InitStack(2, &task2, &task2_stack[0], 1024);
    InitStack(3, &task3, &task3_stack[0], 1024);

    curr_task = 0;
    __set_PSP(PSP_array[curr_task] + 16 * 4);
    NVIC_SetPriority(PendSV_IRQn, 0xFF);
    __set_CONTROL(0x3);
    __ISB();
    task0();

    while (1) {
        __NOP();
    };

    /*while (1) {
        for (uint8_t i = 0; i <= 7; i++) {
            HAL_GPIO_WritePin(KEYBOARD_S0_GPIO_Port, KEYBOARD_S0_Pin, i & 0b00000001U);
            HAL_GPIO_WritePin(KEYBOARD_S1_GPIO_Port, KEYBOARD_S1_Pin, i & 0b00000010U);
            HAL_GPIO_WritePin(KEYBOARD_S2_GPIO_Port, KEYBOARD_S2_Pin, i & 0b00000100U);

            HAL_Delay(10);

            uint16_t raw = readAnalogIn();

            //any of the keys is pressed
            if (raw >= buttonThreshold) {
                HAL_Delay(25);
                raw = readAnalogIn();

                if (raw >= buttonThreshold) {
                    onButtonPressed(i);
                }
            }
        }

    }*/
}

void task0(void) {
    while (1) {
        if (HAL_GetTick() & 0x80) {
            HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
        }
    }
}

void task1(void) {
    while (1) {
        if (HAL_GetTick() & 0x100) {
            HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);
        }
    }
}

void task2(void) {
    while (1) {

    }
}

void task3(void) {
    while (1) {

    }
}

static void onButtonPressed(uint8_t i) {
}

static uint16_t readAnalogIn() {
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
    uint16_t raw = HAL_ADC_GetValue(&hadc1);
    return raw;
}

void Donkos_Init() {
    //Driver init
    HAL_Init();

    //Clock Init
    SystemClock_Config();

    //Digital
    MX_GPIO_Init();

    //Analog In
    MX_ADC1_Init();
}

void SysTick_Handler(void) {
    HAL_IncTick();
    next_task = (curr_task + 1) % 4;
    if (curr_task != next_task) {
        SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    }
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void) {
    context_switch();
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

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOA, KEYBOARD_S0_Pin | KEYBOARD_S1_Pin | KEYBOARD_S2_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOB, LED_1_Pin | LED_2_Pin, GPIO_PIN_RESET);

    /*Configure GPIO pins : KEYBOARD_S0_Pin KEYBOARD_S1_Pin KEYBOARD_S2_Pin */
    GPIO_InitStruct.Pin = KEYBOARD_S0_Pin | KEYBOARD_S1_Pin | KEYBOARD_S2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : LED_1_Pin LED_2_Pin */
    GPIO_InitStruct.Pin = LED_1_Pin | LED_2_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init() {
    ADC_MultiModeTypeDef multimode = {0};
    ADC_ChannelConfTypeDef sConfig = {0};

    /** Common config
    */
    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1.Init.LowPowerAutoWait = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc1.Init.OversamplingMode = DISABLE;
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    /** Configure the ADC multi-mode
    */
    multimode.Mode = ADC_MODE_INDEPENDENT;
    if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Regular Channel
    */
    sConfig.Channel = ADC_CHANNEL_6;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
        Error_Handler();
    }
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
