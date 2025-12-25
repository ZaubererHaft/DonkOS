#include "MockDisplay.h"

void MockDisplay::Init() {
}

void MockDisplay::Display(int32_t page, int32_t line, const char *text) {
}

void MockDisplay::NextPage() {
}

void MockDisplay::Refresh() {
}

void MockDisplay::Clear() {
}

void MockDisplay::DrawPixel(int32_t x, int32_t y) {
}

std::pair<int32_t, int32_t> MockDisplay::GetDimensions() {
    return {0, 0};
}

int32_t MockDisplay::GetCurrentPageIndex() const {
    return 0;
}

void MockDisplay::WriteAt(int32_t x, int32_t y, const char *text) {
}

void MockDisplay::SetCurrentPageDirty() {
}
