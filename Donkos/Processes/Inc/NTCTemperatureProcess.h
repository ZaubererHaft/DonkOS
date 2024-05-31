#ifndef TEST_NTCTEMPERATUREPROCESS_H
#define TEST_NTCTEMPERATUREPROCESS_H

#include "Process.h"
#include "main.h"
#include "ProcessMatrixDisplay.h"

class NTCTemperatureProcess : public Process {
public:
    explicit NTCTemperatureProcess();

    void Main() override;

private:
    static constexpr uint32_t countTemperatures = 1000;

    ADC_HandleTypeDef hadc1;
};


#endif //TEST_NTCTEMPERATUREPROCESS_H
