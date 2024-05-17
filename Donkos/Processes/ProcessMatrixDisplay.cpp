#include "ProcessMatrixDisplay.h"

ProcessMatrixDisplay::ProcessMatrixDisplay() {
    HAL_GPIO_WritePin(DISPLAY_CLK_GPIO_Port, DISPLAY_CLK_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(DISPLAY_DOUT_GPIO_Port, DISPLAY_DOUT_Pin, GPIO_PIN_RESET);

}

void ProcessMatrixDisplay::Main() {

    while (1) {
        HAL_GPIO_WritePin(DISPLAY_DOUT_GPIO_Port, DISPLAY_DOUT_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(DISPLAY_CLK_GPIO_Port, DISPLAY_CLK_Pin, GPIO_PIN_RESET);

        HAL_Delay(1000);

        HAL_GPIO_WritePin(DISPLAY_DOUT_GPIO_Port, DISPLAY_DOUT_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(DISPLAY_CLK_GPIO_Port, DISPLAY_CLK_Pin, GPIO_PIN_SET);
    }
}

