#include "ProcessMatrixDisplay.h"
#include "stm32l4xx_hal.h"

ProcessMatrixDisplay::ProcessMatrixDisplay() : hspi1{} {

    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 7;
    hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;

    if (HAL_SPI_Init(&hspi1) != HAL_OK) {
        Error_Handler();
    }
    SendData(0x0F, 0x01); // display test -> turn all LEDs on
}

void ProcessMatrixDisplay::Main() {
    SendData(0x0F, 0x00); // normal mode
    SendData(0x0B, 0x07); // scan limit
    SendData(0x09, 0x00); // no decode
    SendData(0x0A, 0x00); // intensity

    for (int i = 1; i <= 8; ++i) {
        SendData(i, 0x00); // clear display
    }


    while (true) {
        for (int i = 1; i <= 8; ++i) {
            SendData(i, 0x81);
            HAL_Delay(250);
        }
        for (int i = 8; i >= 1; --i) {
            SendData(i, 0x00);
            HAL_Delay(250);
        }
    }
}

void ProcessMatrixDisplay::SendData(uint8_t addr, uint8_t data) {
    HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);

    HAL_StatusTypeDef result1 = HAL_SPI_Transmit(&hspi1, &addr, 1, 5);
    HAL_StatusTypeDef result2 = HAL_SPI_Transmit(&hspi1, &data, 1, 5);

    if (result1 != HAL_OK || result2 != HAL_OK) {
        Error_Handler();
    }

    HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);

}
