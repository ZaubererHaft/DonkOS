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
    static constexpr int32_t stringBufferSize = 16;

    ADC_HandleTypeDef hadc3;
    NTCSensor sensor;


    static float getADCRefVoltageInV();

    float readTemperatureFromSensor();

    static void temperatureToString(char output_string[stringBufferSize], float measuredTemperature) ;
};


#endif //TEST_ADC3PROCESS_H
