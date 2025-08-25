#include "DHT11NonblockingProcess.h"
#include "DonkosInternal.h"
#include "StringConverter.h"

DHT11NonblockingProcess::DHT11NonblockingProcess() : state{DHT_STATE::IDLE}, htim{} {

}

void DHT11NonblockingProcess::SetHandle(TIM_HandleTypeDef handle) {
    htim = handle;
}

uint32_t bits_received = 0;
uint64_t data_received = 0;

void DHT11NonblockingProcess::Main() {
    while (true) {
        if (state == DHT_STATE::RESTART) {
            bits_received = 0;
            data_received = 0;
            HAL_GPIO_WritePin(DHT_DATA_GPIO_Port, DHT_DATA_Pin, GPIO_PIN_SET);
            wait(1000);
            state = DHT_STATE::IDLE;

        } else if (state == DHT_STATE::IDLE) {

            state = DHT_STATE::COMM_INITIALIZED;

            HAL_GPIO_WritePin(DHT_DATA_GPIO_Port, DHT_DATA_Pin, GPIO_PIN_RESET);

            __HAL_TIM_SET_AUTORELOAD(&htim, 18000 - 1);
            htim.Instance->EGR = TIM_EGR_UG;
            __HAL_TIM_CLEAR_IT(&htim, TIM_DIER_UIE);
            __HAL_TIM_ENABLE(&htim);
            __HAL_TIM_ENABLE_IT(&htim, TIM_DIER_UIE);

        }

        if (state == DHT_STATE::PROCESS_DATA) {

            uint32_t hum1 = data_received & 0xFF00000000 >> 32;
            uint32_t hum2 = data_received & 0x00FF000000 >> 24;
            uint32_t temp1 = data_received & 0x0000FF0000 >> 16;
            uint32_t temp2 = data_received & 0x000000FF00 >> 8;
            uint32_t check = data_received & 0x00000000FF >> 0;

            if (check == hum1 + hum2 + temp1 + temp2) {
                StringConverter converter{};
                char output_temperature_string[11] = "T: ";
                char output_humidity_string[11] = "H: ";

                int32_t string_start_index = 3U;
                int32_t string_max_len = 11 - string_start_index - 2;

                auto [success1, index1] = converter.IntegerToString(static_cast<int32_t>(temp1),
                                                                    &output_temperature_string[string_start_index],
                                                                    string_max_len);
                index1 += string_start_index;
                output_temperature_string[index1] = ' ';
                output_temperature_string[index1 + 1] = 'd';
                output_temperature_string[index1 + 2] = 'C';
                output_temperature_string[index1 + 3] = '\0';

                auto [success2, index2] = converter.IntegerToString(static_cast<int32_t>(hum1),
                                                                    &output_humidity_string[string_start_index],
                                                                    string_max_len);
                index2 += string_start_index;
                output_humidity_string[index2] = ' ';
                output_humidity_string[index2 + 1] = '%';
                output_humidity_string[index2 + 2] = '\0';

                Donkos_Display(2, 2, &output_temperature_string[0]);
                Donkos_Display(2, 3, &output_humidity_string[0]);

                state = DHT_STATE::RESTART;
            } else {
                state = DHT_STATE::COMM_ERROR;
            }

        }

        if (state == DHT_STATE::COMM_ERROR) {
            Donkos_Display(2, 2, "Fehler :(");
            Donkos_Display(2, 3, "");

            state = DHT_STATE::RESTART;
        }
    }
}


void DHT11NonblockingProcess::TimerInterruptReceived() {
    HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_SET);
    __HAL_TIM_DISABLE(&htim);
    __HAL_TIM_SET_COUNTER(&htim, 0);

    int32_t new_time = 0;

    // // called after 18 ms signal low -> now set to high, expect DHT to work immediately
    if (state == DHT_STATE::COMM_INITIALIZED) {
        HAL_GPIO_WritePin(DHT_DATA_GPIO_Port, DHT_DATA_Pin, GPIO_PIN_SET);
        state = DHT_STATE::COMM_INITIALIZED_WAIT;
        new_time = 50;
    }
        // called after 54 us data high. device should have set pin low by now -> now device should set 80 us high
    else if (state == DHT_STATE::COMM_INITIALIZED_WAIT) {
        // now: data coming from dht
        if (HAL_GPIO_ReadPin(DHT_DATA_GPIO_Port, DHT_DATA_Pin) == GPIO_PIN_RESET) {
            state = DHT_STATE::COMM_INITIALIZED_WAIT_FOR_DEVICE;
            new_time = 80;

        } else {
            state = DHT_STATE::COMM_ERROR;
        }
    }
        // called after 80 us. device should set pin to high -> now start read data (starts with 50 us low signal. split in two halves for a check)
    else if (state == DHT_STATE::COMM_INITIALIZED_WAIT_FOR_DEVICE) {

        if (HAL_GPIO_ReadPin(DHT_DATA_GPIO_Port, DHT_DATA_Pin) == GPIO_PIN_SET) {
            state = DHT_STATE::READY_FOR_BIT;
            new_time = 58;
        } else {
            state = DHT_STATE::COMM_ERROR;
        }
    }
        // intermediate check - has the signal been set to low? if yes we can start continue waiting
    else if (state == DHT_STATE::READY_FOR_BIT) {
        if (HAL_GPIO_ReadPin(DHT_DATA_GPIO_Port, DHT_DATA_Pin) == GPIO_PIN_RESET) {
            state = DHT_STATE::READ_BIT;
            new_time = 30;
        } else {
            state = DHT_STATE::COMM_ERROR;
        }
    }
        // successfully waited 50 us on low. now we can start read. check after 30 us again
    else if (state == DHT_STATE::WAIT_FOR_BIT) {
        if (HAL_GPIO_ReadPin(DHT_DATA_GPIO_Port, DHT_DATA_Pin) == GPIO_PIN_SET) {
            state = DHT_STATE::READ_BIT;
            new_time = 30;
        } else {
            state = DHT_STATE::COMM_ERROR;
        }
    }
        // bit arrived - after 30 us. if lane is low, bit is low, otherwise wait longer and it is high
        // after read, wait 25 us for the intermediate check above if more bits expected
    else if (state == DHT_STATE::READ_BIT) {
        int32_t next_wait = 0;
        bits_received++;

        if (HAL_GPIO_ReadPin(DHT_DATA_GPIO_Port, DHT_DATA_Pin) == GPIO_PIN_RESET) {
            data_received <<= 1;
            next_wait = 25 / 2 + 56 / 2;
        } else {
            data_received = (data_received | 1) << 1;
            next_wait = 25 / 2 + 56 / 2 + 30;
        }

        if (bits_received < 40) {
            state = DHT_STATE::READ_BIT;
            new_time = (next_wait);

        } else {
            state = DHT_STATE::COMM_END;
            HAL_GPIO_WritePin(DHT_DATA_GPIO_Port, DHT_DATA_Pin, GPIO_PIN_RESET);
            // end comm by setting lane to low
            new_time = (50 + 56 / 2);
        }
    }
        // comm ended -> now data can be processed
    else if (state == DHT_STATE::COMM_END) {
        HAL_GPIO_WritePin(DHT_DATA_GPIO_Port, DHT_DATA_Pin, GPIO_PIN_SET);
        state = DHT_STATE::PROCESS_DATA;
    }

    if (new_time > 0) {
        // new AAR !! auto reload enable bit must be set
        __HAL_TIM_SET_AUTORELOAD(&htim, new_time - 1);
        htim.Instance->EGR = TIM_EGR_UG;
        __HAL_TIM_CLEAR_IT(&htim, TIM_DIER_UIE);

        // Re-enable the timer (if it was disabled)
        __HAL_TIM_ENABLE(&htim);
    } else {
        htim.Instance->EGR = TIM_EGR_UG;
        __HAL_TIM_DISABLE_IT(&htim, TIM_DIER_UIE);
    }

    HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_RESET);
}

