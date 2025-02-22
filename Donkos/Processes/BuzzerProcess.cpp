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
    //CCRx -> Set By "Pulse"
    //ARRx -> Set by "Period"
    //Duty Cycle: % of period the PWM is in state high -> CCRx / ARRx = 0.5
    //PWM Freq = (TimerClock) / ((ARR + 1) * (PRESCALER + 1))
    //Period (in s) = 1 / PWMFreq

    //Resulting from formula above
    return (TimerClock / ((htim1.Init.Period + 1) * frequency)) - 1;
}


void BuzzerProcess::Main() {
    if (HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2) != HAL_OK) {
        Error_Handler();
    }

    tone(523, 800);

    if (HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2) != HAL_OK) {
        Error_Handler();
    }
}

void BuzzerProcess::tone(uint32_t freq, int32_t ms) {
    __HAL_TIM_SET_PRESCALER(&htim1, prescalerValueForFrequency(freq));
    wait(ms);
}

void BuzzerProcess::noTone(int32_t ms) {
    auto old = htim1.Instance->CCR2;
    htim1.Instance->CCR2 = 0;
    wait(ms);
    htim1.Instance->CCR2 = old;
}

void happyBirthday()
{
    /*tone(396, 180); //g1
tone(396, 200); //g1
tone(440, 400); //a1
tone(396, 400); //g1
tone(523, 400); //c2
tone(493, 800); //h1

tone(396, 180); //g1
tone(396, 200); //g1
tone(440, 400); //a1
tone(396, 400); //g1
tone(587, 400); //d2
tone(523, 800); //c2

tone(396, 180); //g1
tone(396, 200); //g1
tone(783, 400); //g2
tone(659, 400); //e2
tone(523, 400); //c2
tone(493, 400); //h1
tone(440, 400); //a1

tone(698, 180); //f2
tone(698, 200); //f2
tone(659, 400); //e2
tone(523, 400); //c2
tone(587, 400); //d2
tone(523, 800); //c2*/
}

