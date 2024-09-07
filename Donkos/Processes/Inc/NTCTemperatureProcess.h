#ifndef TEST_NTCTEMPERATUREPROCESS_H
#define TEST_NTCTEMPERATUREPROCESS_H

#include "Process.h"
#include "main.h"
#include "ProcessMatrixDisplay.h"
#include "NTCSensor.h"

class NTCTemperatureProcess : public Process {
public:
    explicit NTCTemperatureProcess();

    void Main() override;

    void InitADC();

private:
    static constexpr uint32_t countTemperatures = 100;

    ADC_HandleTypeDef hadc3;
    NTCSensor sensor;
    static float getADCRefVoltageInV();
};


#endif //TEST_NTCTEMPERATUREPROCESS_H
