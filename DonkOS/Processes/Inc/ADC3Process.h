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
    static constexpr int32_t countSamples = 16;
    static constexpr int32_t stringBufferSize = 16;

    ADC_HandleTypeDef hadc3;
    NTCSensor sensor;

    static float getADCRefVoltageInV();

    void readSensors(float data[2]);

    static void temperatureToString(char output_string[stringBufferSize], float measuredTemperature);

    static void lumiToString(char lumiString[stringBufferSize], float voltage);
};


#endif //TEST_ADC3PROCESS_H
