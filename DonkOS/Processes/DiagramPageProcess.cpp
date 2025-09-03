#include <valarray>
#include "DiagramPageProcess.h"

DiagramPageProcess::DiagramPageProcess(BaseDisplay *display) : display{display} {

}

void DiagramPageProcess::Main() {

    auto [xSize, ySize] = display->GetDimensions();
    auto offset = 0.0;

    while (true)
    {
        display->Display(3, 0, " y");

        for (int i = 0; i < xSize; ++i) {
            display->DrawPixel(i, ySize / 2);
        }
        for (int i = 0; i < ySize; ++i) {
            display->DrawPixel(1, i);
        }

        for (int i = 0; i < xSize; ++i) {
            float t = i * 0.1;
            float val = 12.0 * std::sin(t + offset);
            display->DrawPixel(i, (int) val + ySize / 2);
        }

        wait(500);

        display->Clear();
        offset += 0.5;
    }
}
