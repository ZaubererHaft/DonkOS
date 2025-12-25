#ifndef LEDDISPLAY_H
#define LEDDISPLAY_H

#include "BaseDisplay.h"
#include "main.h"
#include "Lock.h"

class Page {
public:
    static constexpr int32_t lines = 4;
    static constexpr int32_t characters = 32;

    char lineBuffers[lines][characters];
    bool dirty;
};

class LedDisplay : public BaseDisplay {
public:
    LedDisplay();

    void Init() override;

    void Display(int32_t page, int32_t line, const char *text) override;

    void Refresh() override;

    void NextPage() override;

    void Clear() override;

    void SetCurrentPageDirty() override;

    void DrawPixel(int32_t x, int32_t y) override;

    std::pair<int32_t, int32_t> GetDimensions() override;

    int32_t GetCurrentPageIndex() const override;

    void WriteAt(int32_t x, int32_t y, const char *text) override;

private:
    static constexpr int32_t count_pages = 5;

    Page pages[count_pages];

    int32_t currentPageIndex;
    int32_t nextPageIndex;
    SimpleLock lock;
    bool enforce_clear;

    bool needsPageChange() const;

    Page &getCurrentPage();
};


#endif //LEDDISPLAY_H
