
#ifndef TEST_PROCESSMATRIXDISPLAY_H
#define TEST_PROCESSMATRIXDISPLAY_H

#include "Process.h"
#include "main.h"
#include "Display.h"

class ProcessMatrixDisplay : public Process {
public:
    ProcessMatrixDisplay(Display *display);

    void Main() override;

private:
    Display *display;
};


#endif //TEST_PROCESSMATRIXDISPLAY_H
