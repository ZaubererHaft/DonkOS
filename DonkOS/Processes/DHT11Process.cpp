#include "DHT11Process.h"
#include "DonkosInternal.h"
#include "StringConverter.h"

DHT11Process::DHT11Process() : htim{} {

}

void DHT11Process::SetHandle(TIM_HandleTypeDef handle) {
    htim = handle;
}

void DHT11Process::Main() {
    HAL_TIM_Base_Start(&htim);

    float temp_Celsius = 0;
    float humidity = 0;

    while (true) {
        char output_temperature_string[11] = "T: ";
        char output_humidity_string[11] = "H: ";
        bool success = false;

        __disable_irq();

        if (initCommunication()) {
            uint8_t hum1, hum2, tempC1, tempC2, SUM, CHECK;

            hum1 = Read();
            hum2 = Read();
            tempC1 = Read();
            tempC2 = Read();
            SUM = Read();
            EndCommunication();

            __enable_irq();

            CHECK = hum1 + hum2 + tempC1 + tempC2;
            if (CHECK == SUM) {
                success = true;
                temp_Celsius = tempC1;
                humidity = hum1;
            }
        } else {
            __enable_irq();
        }

        if (success) {

            StringConverter converter{};

            int32_t string_start_index = 3U;
            int32_t string_max_len = 11 - string_start_index - 2;
            auto [success1, index1] = converter.FloatToString(temp_Celsius,
                                                              &output_temperature_string[string_start_index],
                                                              string_max_len);
            index1 += string_start_index;
            output_temperature_string[index1] = ' ';
            output_temperature_string[index1 + 1] = 'd';
            output_temperature_string[index1 + 2] = 'C';
            output_temperature_string[index1 + 3] = '\0';

            auto [success2, index2] = converter.FloatToString(humidity,
                                                              &output_humidity_string[string_start_index],
                                                              string_max_len);
            index2 += string_start_index;
            output_humidity_string[index2] = ' ';
            output_humidity_string[index2 + 1] = '%';
            output_humidity_string[index2 + 2] = '\0';

            Donkos_Display(2, 2, &output_temperature_string[0]);
            Donkos_Display(2, 3, &output_humidity_string[0]);
        } else {
            Donkos_Display(2, 2, "Keine Werte empfangen :(");
            Donkos_Display(2, 3, "");
        }

        Donkos_YieldProcess(this);
    }
}


bool DHT11Process::initCommunication() {
    bool Response = false;

    //send start signal by pull down voltage at least 18 ms
    setDataOutput();
    HAL_GPIO_WritePin(DHT_DATA_GPIO_Port, DHT_DATA_Pin, GPIO_PIN_RESET);

    for (int i = 0; i < 180; ++i) {
        microDelay(100);
    }

    //now pull up voltage and wait for DHT response 20-40 mu s
    HAL_GPIO_WritePin(DHT_DATA_GPIO_Port, DHT_DATA_Pin, GPIO_PIN_SET);
    setDataInput();
    microDelay(54);

    //DHT should send out response signal for 80 ms...
    if (!(HAL_GPIO_ReadPin(DHT_DATA_GPIO_Port, DHT_DATA_Pin))) {
        microDelay(80);

        // and DHT pulls up voltage and keeps it for 80 ms
        if ((HAL_GPIO_ReadPin(DHT_DATA_GPIO_Port, DHT_DATA_Pin))) {
            Response = true;
        }
    }

    return Response;
}

uint8_t DHT11Process::Read() {
    uint8_t ret = 0;
    for (uint32_t j = 0; j < 8; j++) {
        uint32_t timeout = 0xFFF;

        //wait for pin to go low -> data arrive
        while (HAL_GPIO_ReadPin(DHT_DATA_GPIO_Port, DHT_DATA_Pin) && timeout > 0) {
            timeout--;
        }
        timeout = 0xFFF;
        microDelay(50);
        // pin should be high now ...
        while (!HAL_GPIO_ReadPin(DHT_DATA_GPIO_Port, DHT_DATA_Pin) && timeout > 0) {
            timeout--;
        }

        // wait 28 us
        microDelay(28);

        // pin still high?
        if (HAL_GPIO_ReadPin(DHT_DATA_GPIO_Port, DHT_DATA_Pin)) {
            ret |= (1 << (7 - j));
            microDelay(42);
        }
    }
    return ret;
}

void DHT11Process::EndCommunication() {
    setDataOutput();
    HAL_GPIO_WritePin(DHT_DATA_GPIO_Port, DHT_DATA_Pin, GPIO_PIN_RESET);
    microDelay(50);
    HAL_GPIO_WritePin(DHT_DATA_GPIO_Port, DHT_DATA_Pin, GPIO_PIN_SET);
}


void DHT11Process::microDelay(uint16_t delay) {
    __HAL_TIM_SET_COUNTER(&htim, 0);
    while (__HAL_TIM_GET_COUNTER(&htim) < delay);
}

void DHT11Process::setDataOutput() {
    GPIO_InitTypeDef GPIO_InitStructPrivate = {0};
    GPIO_InitStructPrivate.Pin = DHT_DATA_Pin;
    GPIO_InitStructPrivate.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructPrivate.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(DHT_DATA_GPIO_Port, &GPIO_InitStructPrivate);
}

void DHT11Process::setDataInput() {
    GPIO_InitTypeDef GPIO_InitStructPrivate = {0};
    GPIO_InitStructPrivate.Pin = DHT_DATA_Pin;
    GPIO_InitStructPrivate.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructPrivate.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructPrivate.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(DHT_DATA_GPIO_Port, &GPIO_InitStructPrivate);
}



