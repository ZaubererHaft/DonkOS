#include "Inc/ProcessLed1.h"
#include "main.h"

void ProcessLed1::Execute() {
    while (1) {
        HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET);
        HAL_Delay(400);
        HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);
        HAL_Delay(400);
    }
}
