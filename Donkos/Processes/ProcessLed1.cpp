#include "ProcessLed1.h"
#include "main.h"

void ProcessLed1::Main() {
    while (1) {
        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
        wait(400);
        HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
        wait(400);
    }
}

