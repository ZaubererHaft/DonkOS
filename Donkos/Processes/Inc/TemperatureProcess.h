#ifndef TEST_TEMPERATUREPROCESS_H
#define TEST_TEMPERATUREPROCESS_H

#include "Process.h"
#include "main.h"
#include "ProcessMatrixDisplay.h"

class TemperatureProcess : public Process {
public:
    TemperatureProcess(ProcessMatrixDisplay *display);

    void Main() override;

private:
    ADC_HandleTypeDef hadc1;
    ProcessMatrixDisplay *display;

    static constexpr uint32_t countTemperatures = 10;
    uint8_t lastTemperatures [countTemperatures];
    uint8_t index;
};


#endif //TEST_TEMPERATUREPROCESS_H
