#include <cmath>
#include <string>
#include <cstring>
#include "ADC3Process.h"
#include "main.h"
#include "DonkosInternal.h"

namespace {
    constexpr float ADC_MAX = 4095.0f;
}

ADC3Process::ADC3Process() : hadc3{}, sensor{{10'000.0f, 3835.51}} {
}

void ADC3Process::InitADC() {


    /* USER CODE BEGIN ADC3_Init 0 */

    /* USER CODE END ADC3_Init 0 */

    ADC_ChannelConfTypeDef sConfig = {0};

    /* USER CODE BEGIN ADC3_Init 1 */

    /* USER CODE END ADC3_Init 1 */

    /** Common config
    */
    hadc3.Instance = ADC3;
    hadc3.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV128;
    hadc3.Init.Resolution = ADC_RESOLUTION_12B;
    hadc3.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    hadc3.Init.ScanConvMode = ADC_SCAN_ENABLE;
    hadc3.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
    hadc3.Init.LowPowerAutoWait = DISABLE;
    hadc3.Init.ContinuousConvMode = ENABLE;
    hadc3.Init.NbrOfConversion = 2;
    hadc3.Init.DiscontinuousConvMode = DISABLE;
    hadc3.Init.ExternalTrigConv = ADC_SOFTWARE_START;
    hadc3.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    hadc3.Init.DMAContinuousRequests = ENABLE;
    hadc3.Init.Overrun = ADC_OVR_DATA_PRESERVED;
    hadc3.Init.OversamplingMode = DISABLE;
    if (HAL_ADC_Init(&hadc3) != HAL_OK)
    {
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
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /** Configure Regular Channel
    */
    sConfig.Channel = ADC_CHANNEL_2;
    sConfig.Rank = ADC_REGULAR_RANK_2;
    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
    {
        Error_Handler();
    }
    /* USER CODE BEGIN ADC3_Init 2 */

    /* USER CODE END ADC3_Init 2 */
}

float ADC3Process::getADCRefVoltageInV() {
    return (VREFBUF->CSR & 0b100) == 0 ? 2.048 : 2.5;
}

void ADC3Process::Main() {
    char output_Temp[12] = "Temp: ";
    char output_Lumi[12] = "Lumi: ";

    uint16_t dmaBuffer[2] = {0};

    HAL_ADCEx_Calibration_Start(&hadc3, ADC_SINGLE_ENDED);
    HAL_ADC_Start_DMA(&hadc3, (uint32_t *)&dmaBuffer, 2);
    HAL_ADC_Start(&hadc3);

    while (true) {
        uint32_t rawValueTemperature = 0;
        uint32_t rawValuePhototransistor = 0;

        for (int i = 0; i < countTemperatures; ++i) {
            rawValueTemperature += dmaBuffer[0];
            rawValuePhototransistor += dmaBuffer[1];
            wait(10);
        }

        rawValueTemperature /= countTemperatures;
        rawValuePhototransistor /= countTemperatures;

        // here it gets a little tricky: The reference voltage in the voltage divider circuit is 3.3 V, however IN THE ADC it depends on the value in VREFBUF...
        auto voltageTemperature = (getADCRefVoltageInV() / ADC_MAX) * static_cast<float>(rawValueTemperature);
        auto measuredTemperature = sensor.GetTemperatureInCelsius(voltageTemperature);

        concatToString(output_Temp, measuredTemperature);
        Donkos_SetDisplayLine(0);
        Donkos_Display(&output_Temp[0]);

        auto voltageLumi = (getADCRefVoltageInV() / ADC_MAX) * static_cast<float>(rawValuePhototransistor);
        concatToString(output_Lumi, voltageLumi);
        Donkos_SetDisplayLine(1);
        Donkos_Display(&output_Lumi[0]);
    }
}

void ADC3Process::concatToString(char *output, float measuredTemperature) {
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

