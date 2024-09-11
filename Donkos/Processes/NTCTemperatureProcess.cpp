#include <cmath>
#include <string>
#include <cstring>
#include "NTCTemperatureProcess.h"
#include "main.h"
#include "DonkosInternal.h"

namespace {
    constexpr float ADC_MAX = 4095.0f;
}

NTCTemperatureProcess::NTCTemperatureProcess() : hadc3{}, sensor{{10'000.0f, 3835.51}} {
}

void NTCTemperatureProcess::InitADC() {


    hadc3.Instance = ADC3;
    hadc3.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV128;
    hadc3.Init.Resolution = ADC_RESOLUTION_12B;
    hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc3.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc3.Init.LowPowerAutoWait = DISABLE;
    hadc3.Init.ContinuousConvMode = DISABLE;
    hadc3.Init.NbrOfConversion = 1;
    hadc3.Init.DiscontinuousConvMode = DISABLE;
    hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc3.Init.DMAContinuousRequests = DISABLE;
    hadc3.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc3.Init.OversamplingMode = DISABLE;
    if (HAL_ADC_Init(&hadc3) != HAL_OK) {
        Error_Handler();
    }

    if (HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED) != HAL_OK) {
        Error_Handler();
    }

    ADC_ChannelConfTypeDef sConfig = {0};

    /** Configure Regular Channel
    */
    sConfig.Channel = ADC_CHANNEL_1;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_92CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK) {
        Error_Handler();
    }
}

float NTCTemperatureProcess::getADCRefVoltageInV() {
    return (VREFBUF->CSR & 0b100) == 0 ? 2.048 : 2.5;
}

void NTCTemperatureProcess::Main() {
    char output[12] = "Temp: ";

    while (true) {
        uint32_t rawValueTemperature = 0;
        for (int i = 0; i < countTemperatures; ++i) {
            HAL_ADC_Start(&hadc3);
            HAL_ADC_PollForConversion(&hadc3, 5);
            rawValueTemperature += HAL_ADC_GetValue(&hadc3);
            HAL_ADC_Stop(&hadc3);
        }
        rawValueTemperature /= countTemperatures;

        // here it gets a little tricky: The reference voltage in the voltage divider circuit is 3.3 V, however IN THE ADC it depends on the value in VREFBUF...
        auto voltageTemperature = (getADCRefVoltageInV() / ADC_MAX) * static_cast<float>(rawValueTemperature);
        auto measuredTemperature = sensor.GetTemperatureInCelsius(voltageTemperature);
        concatToString(output, measuredTemperature);
        Donkos_Display(&output[0]);
    }
}

void NTCTemperatureProcess::concatToString(char *output, float measuredTemperature) {
    //cut decimal places and take max. two digits of the integral part
    uint32_t tmp_AsInt = measuredTemperature;
    uint32_t firstPlace = tmp_AsInt / 10;
    uint32_t sndPlace = tmp_AsInt % 10;

    //now take two decimal places and cut rest; suggestion: round number here
    auto workingTemp = measuredTemperature - tmp_AsInt;
    uint32_t firstPlaceAfterDot = workingTemp * 10;
    uint32_t sndPlaceAfterDot = workingTemp * 100 - firstPlaceAfterDot * 10;

    output[6] = 48 + firstPlace;
    output[7] = 48 + sndPlace;
    output[8] = '.';
    output[9] = 48 + firstPlaceAfterDot;
    output[10] = 48 + sndPlaceAfterDot;
    output[11] = '\0';
}
