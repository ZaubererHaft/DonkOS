#ifndef TEST_NTCTEMPERATUREPROCESS_H
#define TEST_NTCTEMPERATUREPROCESS_H

#include "Process.h"
#include "main.h"
#include "ProcessMatrixDisplay.h"

class ClimateData
{
public:
    void AddRawNTC(uint32_t raw);

    float AverageNTCAndReset();

    uint32_t NTCMeasures();

private:
    static constexpr float cal = 50.0;
    static constexpr float offset = 1000.0;

    float ntcTemperature;
    uint32_t ntcMeasures;
};

class NTCTemperatureProcess : public Process {
public:
    explicit NTCTemperatureProcess();

    void Main() override;

private:
    static constexpr uint32_t countTemperatures = 1000;

    ADC_HandleTypeDef hadc1;
    ClimateData data;

    void processNTC();
};


#endif //TEST_NTCTEMPERATUREPROCESS_H
