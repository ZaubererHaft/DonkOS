#include "ProcessLed.h"
#include "main.h"

void ProcessLed::Main() {
    while (1) {
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
        wait(500);
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
        wait(500);
    }
}

