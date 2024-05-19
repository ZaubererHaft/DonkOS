#include "ProcessMatrixDisplay.h"
#include "stm32l4xx_hal.h"

namespace {
    uint8_t s0[] = {0x1F, 0x11, 0x1F};
    uint8_t s1[] = {0x00, 0x00, 0x1F};
    uint8_t s2[] = {0x1D, 0x15, 0x17};
    uint8_t s3[] = {0x15, 0x15, 0x1F};
    uint8_t s4[] = {0x07, 0x04, 0x1F};
    uint8_t s5[] = {0x17, 0x15, 0x1D};
    uint8_t s6[] = {0x1F, 0x15, 0x1D};
    uint8_t s7[] = {0x01, 0x01, 0x1F};
    uint8_t s8[] = {0x1F, 0x15, 0x1F};
    uint8_t s9[] = {0x17, 0x15, 0x1F};
    uint8_t *numbers[] = {&s0[0],
                          &s1[0],
                          &s2[0],
                          &s3[0],
                          &s4[0],
                          &s5[0],
                          &s6[0],
                          &s7[0],
                          &s8[0],
                          &s9[0]};
    uint8_t display1 = 1;
    uint8_t display2 = 5;
}

ProcessMatrixDisplay::ProcessMatrixDisplay() : hspi1{}, numDisplay1{}, numDisplay2{} {

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
    HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
    SendData(0x0C, 0x00); // turn off
}


void ProcessMatrixDisplay::Main() {
    //SendData(0x0F, 0x01); // display test -> turn all LEDs on
    SendData(0x0F, 0x00); // normal mode
    SendData(0x0B, 0x07); // scan limit
    SendData(0x09, 0x00); // no decode
    SendData(0x0A, 0x00); // intensity

    for (int i = 1; i <= 8; ++i) {
        SendData(i, 0x00); // clear display
    }

    SendData(0x0C, 0x01); // turn on

    while (true) {
        Display(display1, numDisplay1);
        Display(display2, numDisplay2);
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

void ProcessMatrixDisplay::Display(uint8_t display, uint8_t number) {
    if (number > 9 || display > 5) {
        Error_Handler();
    }

    uint8_t *segments = numbers[number];

    for (int i = 0; i < 3; ++i) {
        SendData(display + i, segments[i]);
    }
}

void ProcessMatrixDisplay::Display(uint8_t number) {
    numDisplay2 = number % 10;
    numDisplay1 = number / 10;
}
