
#ifndef TEST_PROCESSMATRIXDISPLAY_H
#define TEST_PROCESSMATRIXDISPLAY_H

#include "Process.h"
#include "main.h"

class ProcessMatrixDisplay : public Process {
public:
    ProcessMatrixDisplay();

    void Main() override;

    void Display(uint8_t number);

private:
    SPI_HandleTypeDef hspi1;
    uint8_t numDisplay1;
    uint8_t numDisplay2;

    void SendData(uint8_t addr, uint8_t data);

    void Display(uint8_t display, uint8_t number);

};


#endif //TEST_PROCESSMATRIXDISPLAY_H
