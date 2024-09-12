#include "BuzzerProcess.h"


BuzzerProcess::BuzzerProcess() : htim1{} {

}

void BuzzerProcess::Init() {

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 0;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 999;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_TIM_PWM_Init(&htim1) != HAL_OK) {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 500;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_ENABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_2) != HAL_OK) {
        Error_Handler();
    }
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.BreakFilter = 0;
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
    sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
    sBreakDeadTimeConfig.Break2Filter = 0;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig) != HAL_OK) {
        Error_Handler();
    }

    HAL_TIM_MspPostInit(&htim1);
}


uint32_t BuzzerProcess::prescalerValueForFrequency(uint32_t frequency) {
    if (frequency == 0) return 0;
    //Resulting from formula below
    return (TimerClock / ((htim1.Init.Period + 1) * frequency)) - 1;
}


void BuzzerProcess::Main() {
    if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2) != HAL_OK) {
        Error_Handler();
    }

    //CCR -> Set By "Pulse", here 500
    //ARR -> Set by "Period", here 999
    //Duty Cycle: % of period the PWM is in state high -> CCR / ARR = 0.5
    //PWM Freq = (TimerClock) / ((ARR + 1) * (PRESCALER + 1))
    //Period (in s) = 1 / PWMFreq
    while (1) {
        __HAL_TIM_SET_PRESCALER(&htim1, prescalerValueForFrequency(1000)); //Pres = 3, Period: 1 Ms
        wait(250);
        __HAL_TIM_SET_PRESCALER(&htim1, prescalerValueForFrequency(200)); //Pres = 19, Period: 5 Ms
        wait(250);
        __HAL_TIM_SET_PRESCALER(&htim1, prescalerValueForFrequency(400)); //Pres = 9, Period: 2.5 Ms
        wait(250);
        __HAL_TIM_SET_PRESCALER(&htim1, prescalerValueForFrequency(600)); //Pres = 5, Period: 1.5 Ms
        wait(250);
        __HAL_TIM_SET_PRESCALER(&htim1, prescalerValueForFrequency(800)); // Pes = 4, Period: 1.25 Ms
        wait(250);
    }
}

