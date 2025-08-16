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

    if ((VREFBUF->CSR & VREFBUF_CSR_ENVR_Msk) == 0) {
        return 3.33;
    } else {
        if ((VREFBUF->CSR & VREFBUF_CSR_VRS_Msk) == 0) {
            return 2.048;
        } else {
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

    HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED);

    while (true) {
        //buffer for printing the temperature. always newly initialized to clean up previous conversions
        char output_temperature_string[11] = "T: ";

        float measuredTemperature = readTemperatureFromSensor();
        temperatureToString(output_temperature_string, measuredTemperature);

        Donkos_Display(0, &output_temperature_string[0]);

        wait(10);
    }
}


float ADC3Process::readTemperatureFromSensor() {
    uint32_t rawValue = 0;

    for (int i = 0; i < countTemperatures; ++i) {
        if (HAL_ADC_Start(&hadc3) != HAL_OK) {
            Error_Handler();
        }
        if (HAL_ADC_PollForConversion(&hadc3, 10) != HAL_OK) {
            Error_Handler();
        }
        rawValue += HAL_ADC_GetValue(&hadc3);

        // no need to stop because with continuous mode disabled ADC stops automatically after conversion
    }

    rawValue /= countTemperatures;

    auto voltageTemperature = (getADCRefVoltageInV() / ADC_MAX) * static_cast<float>(rawValue);
    auto measuredTemperature = sensor.GetTemperatureInCelsius(voltageTemperature);
    return measuredTemperature;
}


void ADC3Process::temperatureToString(char output_string[stringBufferSize], float measuredTemperature) {
    StringConverter converter{};

    // the index where the temperature will be printed to. Replaceable with strlen(output_Temp);
    int32_t temp_string_start_index = 3U;
    // how long the temperature string can be. -2 because we want to add °C after the temperature value
    int32_t temp_string_max_len = stringBufferSize - temp_string_start_index - 2;
    auto [success, index] = converter.ToString(measuredTemperature, 2, &output_string[temp_string_start_index],
                                               temp_string_max_len);

    if (success) {
        index += temp_string_start_index;
        output_string[index] = ' ';
        output_string[index + 1] = 'd';
        output_string[index + 2] = 'C';
        output_string[index + 3] = '\0';
    } else {
        Error_Handler();
    }
}