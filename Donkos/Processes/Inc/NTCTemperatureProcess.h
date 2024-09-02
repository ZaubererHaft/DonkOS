#ifndef TEST_NTCTEMPERATUREPROCESS_H
#define TEST_NTCTEMPERATUREPROCESS_H

#include "Process.h"
#include "main.h"
#include "ProcessMatrixDisplay.h"

class NTCTemperatureProcess : public Process {
public:
    explicit NTCTemperatureProcess();

    void Main() override;

    void InitADC();

private:
    static constexpr uint32_t countTemperatures = 100;

    ADC_HandleTypeDef hadc3;

};


#endif //TEST_NTCTEMPERATUREPROCESS_H
