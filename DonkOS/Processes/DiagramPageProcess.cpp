#include <valarray>
#include "DiagramPageProcess.h"

DiagramPageProcess::DiagramPageProcess(LedDisplay *display) : display{display} {
    for (float &i: temp_data_over_time) {
        i = std::numeric_limits<float>::quiet_NaN();
    }
}

void DiagramPageProcess::Main() {

    auto [xSize, ySize] = display->GetDimensions();
    display->Display(3, 0, "Temp over t");

    while (true) {

        for (int i = offset; i < xSize; ++i) {
            display->DrawPixel(i, ySize - 1);
        }
        for (int i = 0; i < ySize; ++i) {
            display->DrawPixel(offset, i);
        }

        display->WriteAt(1, ySize - 11, " 0");

        display->WriteAt(1, ySize - 25 - 3, "25");
        display->DrawPixel(17, ySize - 25);
        display->DrawPixel(18, ySize - 25);
        display->DrawPixel(19, ySize - 25);

        display->WriteAt(1, ySize - 50 - 3, "50");
        display->DrawPixel(17, ySize - 50);
        display->DrawPixel(18, ySize - 50);
        display->DrawPixel(19, ySize - 50);


        for (int i = 0; i < sizeof(temp_data_over_time) / sizeof(float); ++i) {
            if (!std::isnan(temp_data_over_time[i])) {
                float val = temp_data_over_time[i];
                display->DrawPixel(offset + i, ySize - val);
            }
        }
        wait(1000);
        display->Clear();
    }
}

void DiagramPageProcess::PutData(float temperature) {
    temp_data_over_time[index] = temperature;
    index++;
    index %= 128;
}
