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
        char output_lumi_string[11] = "L: ";

        float data[] = {0, 0};

        readSensors(data);
        temperatureToString(output_temperature_string, data[0]);
        lumiToString(output_lumi_string, data[1]);

        Donkos_Display(0, &output_temperature_string[0]);
        Donkos_Display(1, &output_lumi_string[0]);

        wait(10);
    }
}

void ADC3Process::readSensors(float data[2]) {
    uint32_t rawValueTemp = 0;
    uint32_t rawValueFoto = 0;

    for (int i = 0; i < countTemperatures; ++i) {
        if (HAL_ADC_Start(&hadc3) != HAL_OK) {
            Error_Handler();
        }

        if (HAL_ADC_PollForConversion(&hadc3, 10) != HAL_OK) {
            Error_Handler();
        }
        rawValueTemp += HAL_ADC_GetValue(&hadc3);

        if (HAL_ADC_PollForConversion(&hadc3, 10) != HAL_OK) {
            Error_Handler();
        }
        rawValueFoto += HAL_ADC_GetValue(&hadc3);

        // no need to stop because with continuous mode disabled ADC stops automatically after conversion
    }

    rawValueTemp /= countTemperatures;
    rawValueFoto /= countTemperatures;

    auto voltageTemperature = (getADCRefVoltageInV() / ADC_MAX) * static_cast<float>(rawValueTemp);
    auto voltageFoto = (getADCRefVoltageInV() / ADC_MAX) * static_cast<float>(rawValueFoto);

    auto measuredTemperature = sensor.GetTemperatureInCelsius(voltageTemperature);

    data[0] = measuredTemperature;
    data[1] = voltageFoto;
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


void ADC3Process::lumiToString(char output_string[stringBufferSize], float voltage) {
    StringConverter converter{};

    int32_t lumi_string_start_index = 3U;
    int32_t string_max_len = stringBufferSize - lumi_string_start_index - 2;
    auto [success, index] = converter.ToString(voltage, 2, &output_string[lumi_string_start_index],
                                               string_max_len);
    if (success) {
        index += lumi_string_start_index;
        output_string[index] = ' ';
        output_string[index + 1] = 'V';
        output_string[index + 2] = '\0';
    } else {
        Error_Handler();
    }
}