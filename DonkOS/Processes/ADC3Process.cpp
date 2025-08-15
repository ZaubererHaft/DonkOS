#include <cmath>
#include <string>
#include <cstring>
#include "ADC3Process.h"
#include "main.h"
#include "DonkosInternal.h"
#include "StringConverter.h"

namespace {
    constexpr float ADC_MAX = 4095.0f;
}


float ADC3Process::getADCRefVoltageInV() {

    if((VREFBUF->CSR & VREFBUF_CSR_ENVR_Msk) == 0)
    {
        return 3.33;
    }
    else
    {
        if((VREFBUF->CSR & VREFBUF_CSR_VRS_Msk) == 0)
        {
            return 2.048;
        }
        else
        {
            return 2.5;
        }
    }
}

ADC3Process::ADC3Process() : hadc3{}, sensor{{10'000.0f, 3835.51, getADCRefVoltageInV()}} {
}

void ADC3Process::SetHandle(ADC_HandleTypeDef handle) {
    hadc3 = handle;
}

void ADC3Process::Main() {

    StringConverter converter{};

    HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED);

    while (true) {
        uint32_t rawValue= 0;
        char output_Temp[12] = "Temp: ";


        for (int i = 0; i < countTemperatures; ++i) {
            if(HAL_ADC_Start(&hadc3) != HAL_OK)
            {
                Error_Handler();
            }
            if(HAL_ADC_PollForConversion(&hadc3, 100) != HAL_OK)
            {
                Error_Handler();
            }
            rawValue += HAL_ADC_GetValue(&hadc3);

        }

        rawValue /= countTemperatures;

        // here it gets a little tricky: The reference voltage in the voltage divider circuit is 3.3 V, however IN THE ADC it depends on the value in VREFBUF...
        auto voltageTemperature = (getADCRefVoltageInV() / ADC_MAX) * static_cast<float>(rawValue);
        auto measuredTemperature = sensor.GetTemperatureInCelsius(voltageTemperature);

        converter.ToString(measuredTemperature, 2, &output_Temp[6], 6);

        Donkos_SetDisplayLine(0);
        Donkos_Display(&output_Temp[0]);
    }
}

