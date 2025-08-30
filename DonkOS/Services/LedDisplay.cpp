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

    ssd1306_Fill(Black);

    strcpy(pages[0].lineBuffers[0], "Willkommen :)");
    strcpy(pages[0].lineBuffers[2], "Button zum navi-");
    strcpy(pages[0].lineBuffers[3], "gieren benutzen");
    pages[0].dirty = true;

    strcpy(pages[1].lineBuffers[0], "Sensordaten");

    strcpy(pages[2].lineBuffers[0], "DHT-11 Test");
}

void LedDisplay::Clear() {
    ssd1306_Fill(Black);
}


void LedDisplay::Display(int32_t page, int32_t line, const char *text) {
    //ToDo add checks, error handling
    // consider to synchronize writing to this buffer with refreshing to make sure that no partially filled buffers are getting visible when the refresh process interrupts
    // the process calling this method
    lock.Lock();
    strcpy(pages[page].lineBuffers[line], text);
    pages[page].dirty = true;
    lock.Unlock();
}

void LedDisplay::Refresh() {
    lock.Lock();
    if (needsPageChange()) {
        Clear();
        currentPageIndex = nextPageIndex;
    }

    if (getCurrentPage().dirty) {
        ssd1306_SetCursor(120, 53);

        if (currentPageIndex == 2) {
            ssd1306_WriteString("3", Font_7x10, White);
        } else if (currentPageIndex == 1) {
            ssd1306_WriteString("2", Font_7x10, White);
        } else {
            ssd1306_WriteString("1", Font_7x10, White);
        }

        for (int i = 0; i < Page::lines; ++i) {
            ssd1306_SetCursor(i, i * 15);
            ssd1306_WriteString(getCurrentPage().lineBuffers[i], Font_7x10, White);
        }

        ssd1306_UpdateScreen(&hi2c);
        getCurrentPage().dirty = false;
    }
    lock.Unlock();
}

void LedDisplay::NextPage() {
    lock.Unlock();
    nextPageIndex = (currentPageIndex + 1) % count_pages;
    pages[nextPageIndex].dirty = true;
    lock.Unlock();
}

bool LedDisplay::needsPageChange() const {
    return currentPageIndex != nextPageIndex;
}

Page &LedDisplay::getCurrentPage() {
    return pages[currentPageIndex];
}

int32_t LedDisplay::GetCurrentPageIndex() const {
    return currentPageIndex;
}

