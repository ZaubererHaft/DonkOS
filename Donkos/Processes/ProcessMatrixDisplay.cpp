#include "ProcessMatrixDisplay.h"
#include "stm32l4xx_hal.h"

ProcessMatrixDisplay::ProcessMatrixDisplay(Display *display) : display{display} {
}


void ProcessMatrixDisplay::Main() {
    while (true) {
        display->Refresh();
    }
}
