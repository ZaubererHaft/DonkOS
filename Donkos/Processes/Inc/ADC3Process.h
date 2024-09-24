#ifndef TEST_ADC3PROCESS_H
#define TEST_ADC3PROCESS_H

#include "Process.h"
#include "main.h"
#include "ProcessMatrixDisplay.h"
#include "NTCSensor.h"

class ADC3Process : public Process {
public:
    explicit ADC3Process();

    void Main() override;

    void InitADC();

private:
    static constexpr uint32_t countTemperatures = 10;

    ADC_HandleTypeDef hadc3;
    NTCSensor sensor;
    static float getADCRefVoltageInV();

    static void concatToString(char *output, float measuredTemperature) ;
};


#endif //TEST_ADC3PROCESS_H
