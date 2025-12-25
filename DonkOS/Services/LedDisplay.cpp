#include <cstring>
#include "LedDisplay.h"
#include "ssd1306.h"
#include "StringConverter.h"

extern I2C_HandleTypeDef hi2c1;

LedDisplay::LedDisplay() :  pages{}, currentPageIndex{},
                           nextPageIndex{}, enforce_clear{false} {
}

void LedDisplay::Init() {
    if (ssd1306_Init(&hi2c1) != 0) {
        Error_Handler();
    }

    ssd1306_Fill(Black);

    strcpy(pages[0].lineBuffers[0], "Willkommen :)");
    strcpy(pages[0].lineBuffers[2], "Button zum navi-");
    strcpy(pages[0].lineBuffers[3], "gieren benutzen");
    pages[0].dirty = true;

    strcpy(pages[1].lineBuffers[0], "Sensordaten");

    strcpy(pages[2].lineBuffers[0], "DHT-11 Test");

    strcpy(pages[4].lineBuffers[0], "Status");

}

void LedDisplay::Clear() {
    enforce_clear = true;
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
    if (enforce_clear) {
        ssd1306_Fill(Black);
        enforce_clear = false;
    }
    if (needsPageChange()) {
        currentPageIndex = nextPageIndex;
    }

    if (getCurrentPage().dirty) {
        ssd1306_SetCursor(120, 53);

        char page[2];
        StringConverter{}.IntegerToString(currentPageIndex, page, 2);
        ssd1306_WriteString(page, Font_7x10, White);

        for (int i = 0; i < Page::lines; ++i) {
            ssd1306_SetCursor(1, i * 15);
            ssd1306_WriteString(getCurrentPage().lineBuffers[i], Font_7x10, White);
        }

        ssd1306_UpdateScreen(&hi2c1);
        getCurrentPage().dirty = false;
    }
    lock.Unlock();
}

void LedDisplay::NextPage() {
    nextPageIndex = (currentPageIndex + 1) % count_pages;
    pages[nextPageIndex].dirty = true;
    enforce_clear = true;
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

void LedDisplay::DrawPixel(int32_t x, int32_t y) {
    lock.Lock();
    ssd1306_DrawPixel(x, y, SSD1306_COLOR::White);
    lock.Unlock();
}

std::pair<int32_t, int32_t> LedDisplay::GetDimensions() {
    return {SSD1306_WIDTH, SSD1306_HEIGHT};
}

void LedDisplay::WriteAt(int32_t x, int32_t y, const char *text) {
    lock.Lock();
    ssd1306_SetCursor(x, y);
    ssd1306_WriteString(text, Font_7x10, White);
    lock.Unlock();
}

void LedDisplay::SetCurrentPageDirty() {
    pages[nextPageIndex].dirty = true;
}

