#include "DHTProcess.h"

void DHTProcess::microDelay(uint16_t delay) {
    __HAL_TIM_SET_COUNTER(&htim1, 0);
    while (__HAL_TIM_GET_COUNTER(&htim1) < delay);
}

//check https://microcontrollerslab.com/dht22-stm32-blue-pill-stm32cubeide/
DHTProcess::DHTProcess() {
    /* USER CODE BEGIN TIM1_Init 0 */

    /* USER CODE END TIM1_Init 0 */

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

    /* USER CODE BEGIN TIM1_Init 1 */

    /* USER CODE END TIM1_Init 1 */
    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 3;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 65535;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.RepetitionCounter = 0;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }

    HAL_TIM_Base_Start(&htim1);
    /* USER CODE BEGIN TIM1_Init 2 */

    /* USER CODE END TIM1_Init 2 */
}

uint8_t DHTProcess::Start() {
    uint8_t Response = 0;
    GPIO_InitTypeDef GPIO_InitStructPrivate = {0};
    GPIO_InitStructPrivate.Pin = DHT_Pin;
    GPIO_InitStructPrivate.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructPrivate.Pull = GPIO_NOPULL;

    HAL_GPIO_Init(DHT_GPIO_Port, &GPIO_InitStructPrivate); // set the pin as output

    HAL_GPIO_WritePin(DHT_GPIO_Port, DHT_Pin, GPIO_PIN_RESET);   // pull the pin low
    microDelay(1300);   // wait for 1300us
    HAL_GPIO_WritePin(DHT_GPIO_Port, DHT_Pin, GPIO_PIN_SET);   // pull the pin high
    microDelay(30);   // wait for 30us
    GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructPrivate.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT_GPIO_Port, &GPIO_InitStructPrivate); // set the pin as input
    microDelay(40);
    if (!(HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin))) {
        microDelay(80);
        if ((HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin))) Response = 1;
    }
    pMillis = HAL_GetTick();
    cMillis = HAL_GetTick();
    while ((HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin)) && pMillis + 2 > cMillis) {
        cMillis = HAL_GetTick();
    }
    return Response;
}


uint8_t DHTProcess::Read() {
    uint8_t x = 0, y = 0;
    for (x = 0; x < 8; x++) {
        pMillis = HAL_GetTick();
        cMillis = HAL_GetTick();
        while (!(HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin)) && pMillis + 2 > cMillis) {
            cMillis = HAL_GetTick();
        }
        microDelay(40);
        if (!(HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin)))   // if the pin is low
            y &= ~(1 << (7 - x));
        else
            y |= (1 << (7 - x));
        pMillis = HAL_GetTick();
        cMillis = HAL_GetTick();
        while ((HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin)) && pMillis + 2 > cMillis) {  // wait for the pin to go low
            cMillis = HAL_GetTick();
        }
    }
    return y;
}

volatile uint32_t diff = 0;

void DHTProcess::Main() {

    auto start = HAL_GetTick();
    microDelay(5000);
    auto end = HAL_GetTick();

    diff = end - start;

    uint8_t hum1, hum2, tempC1, tempC2, SUM, CHECK;
    float temp_Celsius = 0;
    float temp_Fahrenheit = 0;
    float Humidity = 0;

    if (Start()) {
        while (true) {
            hum1 = Read();
            hum2 = Read();
            tempC1 = Read();
            tempC2 = Read();
            SUM = Read();
            CHECK = hum1 + hum2 + tempC1 + tempC2;
            if (CHECK == SUM) {
                if (tempC1 > 127) {
                    temp_Celsius = (float) tempC2 / 10 * (-1);
                } else {
                    temp_Celsius = (float) ((tempC1 << 8) | tempC2) / 10;
                }
                Humidity = (float) ((hum1<<8)|hum2)/10;
            }
        }
    } else {
        Error_Handler();
    }


}

