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

    void SetHandle(I2C_HandleTypeDef handle);

    int32_t GetCurrentPageIndex() const;

private:
    static constexpr int32_t count_pages = 3;

    I2C_HandleTypeDef hi2c;

    Page pages[count_pages];

    int32_t currentPageIndex;
    int32_t nextPageIndex;
    SimpleLock lock;

    bool needsPageChange() const;

    Page &getCurrentPage();
};


#endif //LEDDISPLAY_H
