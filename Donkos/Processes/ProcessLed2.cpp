//
// Created by ludwig on 4/23/24.
//

#include "Inc/ProcessLed2.h"
#include "main.h"

void ProcessLed2::Execute() {
    while (1) {
        HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
        HAL_Delay(500);
    }
}

ProcessLed2::ProcessLed2(uint32_t pid) : Process(pid) {}
