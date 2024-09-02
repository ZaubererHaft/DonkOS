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


void NTCTemperatureProcess::Main() {

    volatile auto ref = VREFBUF->CSR;
    volatile auto reftype = ref & 0b100;
    volatile float refvolt = reftype == 0 ? 2.048 : 2.5;
    volatile double refCurrent_mA = refvolt / 12.0;

    while (true) {
        double temperature = 0;

        for (int i = 0; i < countTemperatures; ++i) {
            HAL_ADC_Start(&hadc3);
            HAL_ADC_PollForConversion(&hadc3, 5);

            volatile uint32_t raw = HAL_ADC_GetValue(&hadc3);
            HAL_ADC_Stop(&hadc3);

            double voltage_V = (refvolt / 4095.0) * static_cast<double>(raw);
            double resistance_kOhm = (refvolt - voltage_V) / refCurrent_mA;

            double as_Temp = 1.0 / ((std::log(resistance_kOhm / 14.5) / 4300.0) + 1.0 / 289.0) - 273.0;

            temperature += as_Temp;
        }

        temperature /= countTemperatures;
        uint32_t casted = std::roundf(temperature);

        if (casted >= 0 && casted <= 99) {
            Donkos_DisplayNumber(casted);
        } else {
            Error_Handler();
        }
    }
}
