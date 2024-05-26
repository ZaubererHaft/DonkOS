#include "DHTProcess.h"

//check https://microcontrollerslab.com/dht22-stm32-blue-pill-stm32cubeide/
DHTProcess::DHTProcess() {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};

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
}

void DHTProcess::microDelay(uint16_t delay) {
    __HAL_TIM_SET_COUNTER(&htim1, 0);
    while (__HAL_TIM_GET_COUNTER(&htim1) < delay);
}

void DHTProcess::setDataOutput() {
    GPIO_InitTypeDef GPIO_InitStructPrivate = {0};
    GPIO_InitStructPrivate.Pin = DHT_Pin;
    GPIO_InitStructPrivate.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructPrivate.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DHT_GPIO_Port, &GPIO_InitStructPrivate); // set the pin as output
}

void DHTProcess::setDataInput() {
    GPIO_InitTypeDef GPIO_InitStructPrivate = {0};
    GPIO_InitStructPrivate.Pin = DHT_Pin;
    GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructPrivate.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT_GPIO_Port, &GPIO_InitStructPrivate); // set the pin as output
}


uint8_t DHTProcess::Start() {
    setDataOutput();
    HAL_GPIO_WritePin(DHT_GPIO_Port, DHT_Pin, GPIO_PIN_RESET);
    microDelay(19000);
    HAL_GPIO_WritePin(DHT_GPIO_Port, DHT_Pin, GPIO_PIN_SET);
    setDataInput();

    uint8_t Response = 0;
    microDelay(30);
    if (!(HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin))) {
        microDelay(80);
        if ((HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin))) {
            Response = 1;
        }
    }
    pMillis = HAL_GetTick();
    cMillis = HAL_GetTick();
    while ((HAL_GPIO_ReadPin(DHT_GPIO_Port, DHT_Pin)) && pMillis + 2 > cMillis) {
        cMillis = HAL_GetTick();
    }
    return Response;
}


uint8_t DHTProcess::Read() {
    uint8_t i,j;
    for (j=0;j<8;j++)
    {
        while (!(HAL_GPIO_ReadPin (DHT_GPIO_Port, DHT_Pin)));   // wait for the pin to go high
        microDelay(20);   // wait for 40 us
        if (!(HAL_GPIO_ReadPin (DHT_GPIO_Port, DHT_Pin)))   // if the pin is low
        {
            i&= ~(1<<(7-j));   // write 0
        }
        else i|= (1<<(7-j));  // if the pin is high, write 1
        while ((HAL_GPIO_ReadPin (DHT_GPIO_Port, DHT_Pin)));  // wait for the pin to go low
    }
    return i;
}

volatile uint32_t diff = 0;

void DHTProcess::Main() {
    HAL_GPIO_WritePin(DHT_GPIO_Port, DHT_Pin, GPIO_PIN_SET);

    auto start = HAL_GetTick();
    microDelay(50000);
    auto end = HAL_GetTick();

    diff = end - start;

    uint8_t hum1, hum2, tempC1, tempC2, SUM, CHECK;
    volatile float temp_Celsius = 0;
    volatile float temp_Fahrenheit = 0;
    volatile float Humidity = 0;

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
                Humidity = (float) ((hum1 << 8) | hum2) / 10;
            }

            wait(2000);
        }
    } else {
        Error_Handler();
    }


}
