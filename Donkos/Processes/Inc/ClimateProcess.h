#ifndef TEST_CLIMATEPROCESS_H
#define TEST_CLIMATEPROCESS_H

#include "Process.h"
#include "main.h"
#include "ProcessMatrixDisplay.h"

class ClimateProcess : public Process {
public:
    explicit ClimateProcess();

    void Main() override;

private:
    static constexpr uint32_t countTemperatures = 1000;

    ADC_HandleTypeDef hadc1;
};


#endif //TEST_CLIMATEPROCESS_H
