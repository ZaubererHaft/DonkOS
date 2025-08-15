#ifndef TEST_DISPLAY_H
#define TEST_DISPLAY_H


#include "main.h"

class Display {
public:
    Display();

    void Initialize();

    void Refresh();

    void Show(uint8_t number);

    void TearDown();

private:
    //SPI_HandleTypeDef hspi1;
    uint8_t numLeftBuffer;
    uint8_t numRightBuffer;

    void sendData(uint8_t addr, uint8_t data);

    void show(uint8_t display, uint8_t number);
};


#endif //TEST_DISPLAY_H
