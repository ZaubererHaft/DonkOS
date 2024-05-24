#ifndef TEST_TEMPERATUREPROCESS_H
#define TEST_TEMPERATUREPROCESS_H

#include "Process.h"
#include "main.h"
#include "ProcessMatrixDisplay.h"

class TemperatureProcess : public Process {
public:
    explicit TemperatureProcess();

    void Main() override;

private:
    static constexpr uint32_t countTemperatures = 1000;

    ADC_HandleTypeDef hadc1;
};


#endif //TEST_TEMPERATUREPROCESS_H
