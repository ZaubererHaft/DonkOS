#include <cmath>
#include "NTCTemperatureProcess.h"
#include "main.h"
#include "DonkosInternal.h"


NTCTemperatureProcess::NTCTemperatureProcess() : hadc1{}  {
    ADC_MultiModeTypeDef multimode = {0};

    hadc1.Instance = ADC1;
    hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
    hadc1.Init.Resolution = ADC_RESOLUTION_12B;
    hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
    hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc1.Init.LowPowerAutoWait = DISABLE;
    hadc1.Init.ContinuousConvMode = DISABLE;
    hadc1.Init.NbrOfConversion = 1;
    hadc1.Init.DiscontinuousConvMode = DISABLE;
    hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc1.Init.DMAContinuousRequests = DISABLE;
    hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc1.Init.OversamplingMode = DISABLE;
    if (HAL_ADC_Init(&hadc1) != HAL_OK) {
        Error_Handler();
    }

    /** Configure the ADC multi-mode
    */
    multimode.Mode = ADC_MODE_INDEPENDENT;
    if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK) {
        Error_Handler();
    }

    ADC_ChannelConfTypeDef sConfigChannel4;
    sConfigChannel4.Channel = ADC_CHANNEL_4;
    sConfigChannel4.Rank = ADC_REGULAR_RANK_1;
    sConfigChannel4.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
    sConfigChannel4.SingleDiff = ADC_SINGLE_ENDED;
    sConfigChannel4.OffsetNumber = ADC_OFFSET_NONE;
    sConfigChannel4.Offset = 0;

    ADC_ChannelConfTypeDef sConfigChannel1;
    sConfigChannel1.Channel = ADC_CHANNEL_1;
    sConfigChannel1.Rank = ADC_REGULAR_RANK_1;
    sConfigChannel1.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
    sConfigChannel1.SingleDiff = ADC_SINGLE_ENDED;
    sConfigChannel1.OffsetNumber = ADC_OFFSET_NONE;
    sConfigChannel1.Offset = 0;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfigChannel1) != HAL_OK) {
        Error_Handler();
    }
    if (HAL_ADC_ConfigChannel(&hadc1, &sConfigChannel4) != HAL_OK) {
        Error_Handler();
    }
}

void NTCTemperatureProcess::Main() {
    while (true) {
        float cal = 70.0;
        float offset = 1000.0;
        float temp = 0.0;

        for (int i = 0; i < countTemperatures; ++i) {
            HAL_ADC_Start(&hadc1);
            HAL_ADC_PollForConversion(&hadc1, 5);
            uint32_t raw = HAL_ADC_GetValue(&hadc1);
            temp += (static_cast<float>(raw) - offset) / cal;
        }

        temp /= countTemperatures;
        uint8_t casted = std::roundf(temp);

        if (casted >= 0 && casted <= 99) {
            Donkos_DisplayNumber(casted);
        } else {
            Error_Handler();
        }
    }
}
