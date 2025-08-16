#include <cstring>
#include "LedDisplay.h"
#include "ssd1306.h"

LedDisplay::LedDisplay() : hi2c{}, dirty{true}, lineBuffers{{'\0'}} {
}

void LedDisplay::SetHandle(I2C_HandleTypeDef handle) {
    hi2c = handle;
}

void LedDisplay::Init() {
    if (ssd1306_Init(&hi2c) != 0) {
        Error_Handler();
    }
}

void LedDisplay::Display(int32_t line, const char *text) {
    //ToDo add checks, error handling
    strcpy(lineBuffers[line], text);
    dirty = true;
}

void LedDisplay::Refresh() {
    for (int i = 0; i < 3; ++i) {
        ssd1306_SetCursor(i, i * 15);
        ssd1306_WriteString(lineBuffers[i], Font_7x10, White);
    }
    ssd1306_UpdateScreen(&hi2c);
    dirty = false;
}

bool LedDisplay::Dirty() const {
    return dirty;
}
