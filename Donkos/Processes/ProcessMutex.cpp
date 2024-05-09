#include "ProcessMutex.h"
#include "main.h"

void ProcessMutex::Main() {
    while (1) {
        //for (uint8_t muxPin = 0; muxPin <= 7; muxPin++) {

        uint8_t muxPin = 7;
                                                                                                                                                                    GPIO_PinState s1 = static_cast<GPIO_PinState>((muxPin & 0b00000001U) >> 0);
            GPIO_PinState s2 = static_cast<GPIO_PinState>((muxPin & 0b00000010U) >> 1);
            GPIO_PinState s3 = static_cast<GPIO_PinState>((muxPin & 0b00000100U) >> 2);

            HAL_GPIO_WritePin(GPIOA, KEYBOARD_S0_Pin, s1);
            HAL_GPIO_WritePin(GPIOA, KEYBOARD_S1_Pin, s2);
            HAL_GPIO_WritePin(GPIOA, KEYBOARD_S2_Pin, s3);
            HAL_Delay(500);
        //}
    }
}

