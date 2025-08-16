#ifndef LEDDISPLAY_H
#define LEDDISPLAY_H

#include "BaseDisplay.h"
#include "main.h"

class LedDisplay : public BaseDisplay {
public:
    LedDisplay();

    void Init() override;

    void Display(int32_t line, const char *text) override;

    void Refresh() override;

    bool Dirty() const override;

    void SetHandle(I2C_HandleTypeDef handle);

private:
    I2C_HandleTypeDef hi2c;
    bool dirty;

    char lineBuffers[3][64];
};


#endif //TEST_LEDDISPLAY_H
