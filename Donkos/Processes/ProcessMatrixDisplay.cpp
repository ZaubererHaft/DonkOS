#include "ProcessMatrixDisplay.h"
#include "stm32l4xx_hal.h"

#define REG_SHUTDOWN 0x0C
#define REG_INTENSITY 0x0A
#define REG_SHUTDOWN 0x0C
#define REG_SCAN_LIMIT 0x0B
#define NUMBER_OF_DIGITS    8

static uint8_t SYMBOLS[] = {
        0x7E,    // numeric 0
        0x30,    // numeric 1
        0x6D,    // numeric 2
        0x79,    // numeric 3
        0x33,    // numeric 4
        0x5B,    // numeric 5
        0x5F,    // numeric 6
        0x70,    // numeric 7
        0x7F,    // numeric 8
        0x7B,    // numeric 9
        0x01,    // minus
        0x4F,    // letter E
        0x37,    // letter H
        0x0E,    // letter L
        0x67,    // letter P
        0x00    // blank
};


ProcessMatrixDisplay::ProcessMatrixDisplay() : hspi1{}, decodeMode{0U} {

    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_4BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
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

}

void ProcessMatrixDisplay::Main() {

    SendData(0x0CU, 0x01); // power up
    SendData(0x09U, 0x00); // no decode
    SendData(0x0BU, 0x07); // no scan limit
    SendData(0x0AU, 0x07); // medium itensity

    while (1) {
        HAL_Delay(500);
    }
}

void ProcessMatrixDisplay::Clean() {
    uint8_t clear = 0x00;

    if (decodeMode == 0xFF) {
        clear = BLANK;
    }

    for (int i = 0; i < 8; ++i) {
        SendData(i + 1, clear);
    }
}

void ProcessMatrixDisplay::SetIntensivity(uint8_t intensivity) {
    if (intensivity > 0x0F) {
        return;
    }
    SendData(REG_INTENSITY, intensivity);
}

void ProcessMatrixDisplay::TurnOn() {
    SendData(REG_SHUTDOWN, 0x01);
}

void ProcessMatrixDisplay::TurnOff() {
    SendData(REG_SHUTDOWN, 0x00);
}

static uint16_t getSymbol(uint8_t number) {
    return SYMBOLS[number];
}


void ProcessMatrixDisplay::PrintDigit(MAX7219_Digits position, MAX7219_Numeric numeric, bool point) {
    if (position > NUMBER_OF_DIGITS) {
        return;
    }

    if (point) {
        if (decodeMode == 0x00) {
            SendData(position, getSymbol(numeric) | (1 << 7));
        } else if (decodeMode == 0xFF) {
            SendData(position, numeric | (1 << 7));
        }
    } else {
        if (decodeMode == 0x00) {
            SendData(position, getSymbol(numeric) & (~(1 << 7)));
        } else if (decodeMode == 0xFF) {
            SendData(position, numeric & (~(1 << 7)));
        }
    }
}


void ProcessMatrixDisplay::SendData(uint8_t addr, uint8_t data) {
    CS_Set();
    HAL_StatusTypeDef result1 = HAL_SPI_Transmit(&hspi1, &addr, 1, 100);
    HAL_StatusTypeDef result2 = HAL_SPI_Transmit(&hspi1, &data, 1, 100);

    if (result1 != HAL_OK || result2 != HAL_OK) {
        Error_Handler();
    }

    CS_Reset();
}

void ProcessMatrixDisplay::CS_Set() {
    HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_SET);
}

void ProcessMatrixDisplay::CS_Reset() {
    HAL_GPIO_WritePin(DISPLAY_CS_GPIO_Port, DISPLAY_CS_Pin, GPIO_PIN_RESET);
}

