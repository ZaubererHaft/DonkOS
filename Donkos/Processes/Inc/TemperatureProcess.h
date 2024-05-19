#ifndef TEST_TEMPERATUREPROCESS_H
#define TEST_TEMPERATUREPROCESS_H

#include "Process.h"
#include "main.h"

class TemperatureProcess : public Process {
public:
    TemperatureProcess();

    void Main() override;

private:
    ADC_HandleTypeDef hadc1;
};


#endif //TEST_TEMPERATUREPROCESS_H
