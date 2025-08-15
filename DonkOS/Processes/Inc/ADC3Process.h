#ifndef TEST_ADC3PROCESS_H
#define TEST_ADC3PROCESS_H

#include "Process.h"
#include "main.h"
#include "ProcessMatrixDisplay.h"
#include "NTCSensor.h"

class ADC3Process : public Process {
public:
    explicit ADC3Process();

    void SetHandle(ADC_HandleTypeDef handle);

    void Main() override;


private:
    static constexpr uint32_t countTemperatures = 10;

    ADC_HandleTypeDef hadc3;
    NTCSensor sensor;


    static float getADCRefVoltageInV();

};


#endif //TEST_ADC3PROCESS_H
