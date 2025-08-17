#include <cstring>
#include "LedDisplay.h"
#include "ssd1306.h"

LedDisplay::LedDisplay() : hi2c{}, dirty{true}, lineBuffers{{{'\0'}}}, pageChange{true} {
}

void LedDisplay::SetHandle(I2C_HandleTypeDef handle) {
    hi2c = handle;
}

void LedDisplay::Init() {
    if (ssd1306_Init(&hi2c) != 0) {
        Error_Handler();
    }


    strcpy(lineBuffers[1][0], "Welcome!");
    strcpy(lineBuffers[1][1], "Status: good");

}

void LedDisplay::Display(int32_t page, int32_t line, const char *text) {
    //ToDo add checks, error handling
    strcpy(lineBuffers[page][line], text);
    dirty = true;
}

void LedDisplay::Refresh() {
    if (pageChange) {
        ssd1306_Fill(Black);
        pageChange = false;
    }

    for (int i = 0; i < lines; ++i) {
        ssd1306_SetCursor(i, i * 15);
        ssd1306_WriteString(lineBuffers[currentPage][i], Font_7x10, White);
    }
    ssd1306_DrawPixel(120, 30, White);
    ssd1306_DrawPixel(121, 30, White);
    ssd1306_DrawPixel(122, 30, White);

    ssd1306_UpdateScreen(&hi2c);
    dirty = false;
}

bool LedDisplay::Dirty() const {
    return dirty;
}

void LedDisplay::NextPage() {
    currentPage = (currentPage + 1) % pages;
    pageChange = true;
}
