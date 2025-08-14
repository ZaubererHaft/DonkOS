#include "SSD1306Process.h"
#include "main.h"
#include "ssd1306.h"
#include "fonts.h"

I2C_HandleTypeDef hi2c2;

void MX_I2C1_Init() {
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

}


void SSD1306Process::Init() {
    MX_I2C1_Init();

    ssd1306_Init(&hi2c2);


}


void SSD1306Process::Main() {
    ssd1306_SetCursor(0, 0);
    ssd1306_WriteString("Wanna play", Font_11x18, White);
    ssd1306_SetCursor(0, 16);
    ssd1306_WriteString("Minecraft?", Font_11x18, White);

    ssd1306_UpdateScreen(&hi2c2);

    while (true) {
    }
}
