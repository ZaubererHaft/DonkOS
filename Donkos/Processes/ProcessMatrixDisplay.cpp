#include "ProcessMatrixDisplay.h"
#include "stm32l4xx_hal.h"

ProcessMatrixDisplay::ProcessMatrixDisplay(Display *display) : display{display} {
}


void ProcessMatrixDisplay::Main() {
    display->Initialize();

    while (true) {
        display->Refresh();
    }
}
