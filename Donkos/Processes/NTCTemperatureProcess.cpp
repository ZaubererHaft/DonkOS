#include <cmath>
#include "NTCTemperatureProcess.h"
#include "main.h"
#include "DonkosInternal.h"


NTCTemperatureProcess::NTCTemperatureProcess() : hadc3{} {
}

void NTCTemperatureProcess::InitADC() {

    ADC_ChannelConfTypeDef sConfig = {0};

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

float getADCRefVoltageInV() {
    return (VREFBUF->CSR & 0b100) == 0 ? 2.048 : 2.5;
}

void NTCTemperatureProcess::Main() {

    static constexpr double OFFSET_KELVIN = 273.15;
    static constexpr double R25 = 10'000;
    static constexpr double T25 = 25 + OFFSET_KELVIN;
    static constexpr double R1 = 10'000;
    static constexpr double beta = 3835.51;
    static constexpr double voltref_Circuit = 3.3;
    static constexpr double ADC_MAX = 4095;

    while (true) {
        volatile uint32_t raw = 0;
        for (int i = 0; i < countTemperatures; ++i) {
            HAL_ADC_Start(&hadc3);
            HAL_ADC_PollForConversion(&hadc3, 5);
            raw += HAL_ADC_GetValue(&hadc3);
            HAL_ADC_Stop(&hadc3);
        }
        raw /= countTemperatures;

        // Formula inspired from texas instrument: https://www.ti.com/lit/an/sbaa338a/sbaa338a.pdf?ts=1725297395650&ref_url=https%253A%252F%252Fwww.google.com%252F
        // here it gets a little tricky: The reference voltage in the voltage divider circuit is 3.3 V, however IN THE ADC it depends on the value in VREFBUF...
        double v0 = (getADCRefVoltageInV() / ADC_MAX) * raw; //voltage measured by ADC
        double as_Temp = (1.0 / ((1.0 / T25) + (1.0 / beta) * std::log((R1 * v0) / (R25 * (voltref_Circuit - v0))))) -
                         OFFSET_KELVIN;
        auto casted = static_cast<int32_t>(std::round(as_Temp));

        if (casted < 0 || casted > 99) {
            casted = 0;
        }

        Donkos_DisplayNumber(casted);

    }
}
