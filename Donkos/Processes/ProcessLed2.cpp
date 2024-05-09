#include "ProcessLed2.h"
#include "main.h"

void ProcessLed2::Main() {
    while (1) {
        HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
        HAL_Delay(500);
    }
}

