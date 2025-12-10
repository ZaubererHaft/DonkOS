#include "ATHAL.h"
#include "main.h"
#include "DonkosInternal.h"

extern UART_HandleTypeDef huart5;

void ATHAL_EnableChip() {
    HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin, GPIO_PIN_SET);
}

void ATHAL_DisableChip() {
    HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin, GPIO_PIN_RESET);
}

bool ATHAL_StartUARTReceiveIT(uint8_t *data) {
    return HAL_UART_Receive_IT(&huart5, data, 1) == HAL_OK;
}

void ATHAL_Wait(int32_t ms) {
    Donkos_SleepCurrentProcess(ms);
}

bool ATHAL_Transmit(const uint8_t *ptr, int32_t length) {
    return HAL_UART_Transmit(&huart5, ptr, length, 100) == HAL_OK;
}
