#include <valarray>
#include "DiagramPageProcess.h"

DiagramPageProcess::DiagramPageProcess(BaseDisplay *display) : display{display}, temp_data_over_time{}, index{} {
    for (float &i: temp_data_over_time) {
        i = std::numeric_limits<float>::quiet_NaN();
    }
}

void DiagramPageProcess::Main() {
    constexpr float A = 15;
    constexpr float B = 30;
    constexpr float C = 0;
    constexpr float D = 64;

    auto [xSize, ySize] = display->GetDimensions();
    display->Display(3, 0, "deg C ueber t");

    do {
        wait(100);

        for (int i = offset; i < xSize; ++i) {
            display->DrawPixel(i, ySize - 1);
        }
        for (int i = 0; i < ySize; ++i) {
            display->DrawPixel(offset, i);
        }

        display->WriteAt(1, ySize - 11, "15");

        display->WriteAt(1, ySize / 2 - 1, "22");
        display->DrawPixel(17, ySize / 2 + 2);
        display->DrawPixel(18, ySize / 2 + 2);
        display->DrawPixel(19, ySize / 2 + 2);

        volatile int32_t temp_index = index;

        for (int i = 0; i < data_capacity; ++i) {
            int32_t local_index = (i + temp_index) % data_capacity;
            if (!std::isnan(temp_data_over_time[local_index])) {
                float value = temp_data_over_time[local_index];

                // map value in [A, B] = [15, 25] to [0, 64] = [C, D]
                if (value >= A && value <= B) {
                    auto value_mapped = (value - A) / (B - A) * (D - C) + C;
                    display->DrawPixel(offset + i, ySize - value_mapped);
                }
            }
        }

        display->SetCurrentPageDirty();

        wait(490);
        display->Clear();
    } while (true);
}

void DiagramPageProcess::PutData(float temperature) {
    temp_data_over_time[index] = temperature;
    index++;
    index %= data_capacity;
}
