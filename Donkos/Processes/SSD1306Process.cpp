
#include "SSD1306Process.h"
#include "main.h"

I2C_HandleTypeDef hi2c2;

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
void MX_I2C1_Init(void) {

    /* USER CODE BEGIN I2C2_Init 0 */

    /* USER CODE END I2C2_Init 0 */

    /* USER CODE BEGIN I2C2_Init 1 */

    /* USER CODE END I2C2_Init 1 */
    hi2c2.Instance = I2C2;
    hi2c2.Init.Timing = 0x00000E14;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2c2) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Analogue filter
    */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
        Error_Handler();
    }

    /** Configure Digital filter
    */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK) {
        Error_Handler();
    }
    /* USER CODE BEGIN I2C2_Init 2 */

    /* USER CODE END I2C2_Init 2 */

}

#define SSD1306_I2C_ADDR        (0x3C << 1)
#define SSD1306_WIDTH           128
#define SSD1306_HEIGHT          64

static uint8_t SSD1306_Buffer[SSD1306_WIDTH * SSD1306_HEIGHT / 8];

//
//  Enumeration for screen colors
//
typedef enum {
    Black = 0x00,   // Black color, no pixel
    White = 0x01,   // Pixel is set. Color depends on LCD
} SSD1306_COLOR;

void ssd1306_Fill(SSD1306_COLOR color)
{
    // Fill screenbuffer with a constant value (color)
    uint32_t i;

    for(i = 0; i < sizeof(SSD1306_Buffer); i++)
    {
        SSD1306_Buffer[i] = (color == Black) ? 0x00 : 0xFF;
    }
}

void ssd1306_WriteCommand(uint8_t command) {
    HAL_StatusTypeDef ret = HAL_I2C_Mem_Write(&hi2c2, SSD1306_I2C_ADDR, 0x00, 1, &command, 1, 10);

    if (ret != HAL_OK) {
        Error_Handler();
    }
}

//
//  Write the screenbuffer with changed to the screen
//
void ssd1306_UpdateScreen()
{
    uint8_t i;

    for (i = 0; i < 8; i++) {
        ssd1306_WriteCommand(0xB0 + i);
        ssd1306_WriteCommand(0x00);
        ssd1306_WriteCommand(0x10);

        HAL_I2C_Mem_Write(&hi2c2, SSD1306_I2C_ADDR, 0x40, 1, &SSD1306_Buffer[SSD1306_WIDTH * i], SSD1306_WIDTH, 100);
    }
}

void SSD1306Process::Main() {
    MX_I2C1_Init();

    wait(100);

    ssd1306_WriteCommand(0xAE);   // Display off

    ssd1306_WriteCommand(0x20);   // Set Memory Addressing Mode
    ssd1306_WriteCommand(0x10);   // 00,Horizontal Addressing Mode;01,Vertical Addressing Mode;10,Page Addressing Mode (RESET);11,Invalid
    ssd1306_WriteCommand(0xB0);   // Set Page Start Address for Page Addressing Mode,0-7
    ssd1306_WriteCommand(0xC8);   // Set COM Output Scan Direction
    ssd1306_WriteCommand(0x00);   // Set low column address
    ssd1306_WriteCommand(0x10);   // Set high column address
    ssd1306_WriteCommand(0x40);   // Set start line address
    ssd1306_WriteCommand(0x81);   // set contrast control register
    ssd1306_WriteCommand(0xFF);
    ssd1306_WriteCommand(0xA1);   // Set segment re-map 0 to 127
    ssd1306_WriteCommand(0xA6);   // Set normal display

    ssd1306_WriteCommand(0xA8);   // Set multiplex ratio(1 to 64)
    ssd1306_WriteCommand(64 - 1);

    ssd1306_WriteCommand(0xA4);   // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    ssd1306_WriteCommand(0xD3);   // Set display offset
    ssd1306_WriteCommand(0x00);   // No offset
    ssd1306_WriteCommand(0xD5);   // Set display clock divide ratio/oscillator frequency
    ssd1306_WriteCommand(0xF0);   // Set divide ratio
    ssd1306_WriteCommand(0xD9);   // Set pre-charge period
    ssd1306_WriteCommand(0x22);

    ssd1306_WriteCommand(0xDA);   // Set com pins hardware configuration
    ssd1306_WriteCommand(0 << 5 | 1 << 4 | 0x02);

    ssd1306_WriteCommand(0xDB);   // Set vcomh
    ssd1306_WriteCommand(0x20);   // 0x20,0.77xVcc
    ssd1306_WriteCommand(0x8D);   // Set DC-DC enable
    ssd1306_WriteCommand(0x14);   //
    ssd1306_WriteCommand(0xAF);   // Turn on SSD1306 panel


    while (true) {
        ssd1306_Fill(White);
        ssd1306_UpdateScreen();
        wait(1000);
        ssd1306_Fill(Black);
        ssd1306_UpdateScreen();
        wait(1000);
    }
}
