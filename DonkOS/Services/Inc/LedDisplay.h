#ifndef LEDDISPLAY_H
#define LEDDISPLAY_H

#include "main.h"

class LedDisplay {
public:
    LedDisplay();

    void Init();

    void Display(const char *text);

    void Refresh();

    void SetLine(uint32_t line);

private:
    I2C_HandleTypeDef hi2c2;
};


#endif //TEST_LEDDISPLAY_H
