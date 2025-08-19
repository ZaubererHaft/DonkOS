#include <cstring>
#include "LedDisplay.h"
#include "ssd1306.h"

LedDisplay::LedDisplay() : hi2c{}, pages{}, currentPageIndex{},
                           nextPageIndex{} {
}

void LedDisplay::SetHandle(I2C_HandleTypeDef handle) {
    hi2c = handle;
}

void LedDisplay::Init() {
    if (ssd1306_Init(&hi2c) != 0) {
        Error_Handler();
    }

    strcpy(pages[1].lineBuffers[0], "Welcome!");
    strcpy(pages[1].lineBuffers[1], "Status: good");

}

void LedDisplay::Display(int32_t page, int32_t line, const char *text) {
    //ToDo add checks, error handling
    // consider to synchronize writing to this buffer with refreshing to make sure that no partially filled buffers are getting visible when the refresh process interrupts
    // the process calling this method
    strcpy(pages[page].lineBuffers[line], text);
    pages[page].dirty = true;
}

void LedDisplay::Refresh() {

    if (needsPageChange()) {
        ssd1306_Fill(Black);
        currentPageIndex = nextPageIndex;
    }

    if (getCurrentPage().dirty) {
        ssd1306_SetCursor(105, 52);

        if (currentPageIndex) {
            ssd1306_WriteString("2/2", Font_7x10, White);
        } else {
            ssd1306_WriteString("1/2", Font_7x10, White);
        }

        for (int i = 0; i < Page::lines; ++i) {
            ssd1306_SetCursor(i, i * 15);
            ssd1306_WriteString(getCurrentPage().lineBuffers[i], Font_7x10, White);
        }

        ssd1306_UpdateScreen(&hi2c);
        getCurrentPage().dirty = false;
    }
}

void LedDisplay::NextPage() {
    nextPageIndex = (currentPageIndex + 1) % count_pages;
    pages[nextPageIndex].dirty = true;
}

bool LedDisplay::needsPageChange() const {
    return currentPageIndex != nextPageIndex;
}

Page &LedDisplay::getCurrentPage() {
    return pages[currentPageIndex];
}
