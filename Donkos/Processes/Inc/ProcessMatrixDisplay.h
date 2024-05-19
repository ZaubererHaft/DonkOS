
#ifndef TEST_PROCESSMATRIXDISPLAY_H
#define TEST_PROCESSMATRIXDISPLAY_H

#include "Process.h"
#include "main.h"

class ProcessMatrixDisplay : public Process {
public:
    ProcessMatrixDisplay();

    void Main() override;

private:
    SPI_HandleTypeDef hspi1;

    void SendData(uint8_t addr, uint8_t data);
};


#endif //TEST_PROCESSMATRIXDISPLAY_H
