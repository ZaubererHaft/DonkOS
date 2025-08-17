#ifndef LEDDISPLAY_H
#define LEDDISPLAY_H

#include "BaseDisplay.h"
#include "main.h"

class LedDisplay : public BaseDisplay {
public:
    LedDisplay();

    void Init() override;

    void Display(int32_t page, int32_t line, const char *text) override;

    void Refresh() override;

    void NextPage() override;

    bool Dirty() const override;

    void SetHandle(I2C_HandleTypeDef handle);

private:
    static constexpr int32_t pages = 2;
    static constexpr int32_t lines = 4;
    static constexpr int32_t characters = 32;

    I2C_HandleTypeDef hi2c;
    bool dirty;
    int32_t currentPage;

    char lineBuffers[pages][lines][characters];
    bool pageChange;
};


#endif //TEST_LEDDISPLAY_H
