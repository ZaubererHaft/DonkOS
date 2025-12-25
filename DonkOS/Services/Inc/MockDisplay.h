#ifndef DONKOS_MOCKDISPLAY_H
#define DONKOS_MOCKDISPLAY_H

#include "BaseDisplay.h"

class MockDisplay : public BaseDisplay {
public:
    void Init() override;

    void Display(int32_t page, int32_t line, const char *text) override;

    void NextPage() override;

    void Refresh() override;

    void Clear() override;

    void DrawPixel(int32_t x, int32_t y) override;

    std::pair<int32_t, int32_t> GetDimensions() override;

    int32_t GetCurrentPageIndex() const override;

    void WriteAt(int32_t x, int32_t y, const char *text) override;

    void SetCurrentPageDirty() override;
};


#endif //DONKOS_MOCKDISPLAY_H
