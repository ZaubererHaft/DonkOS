#include <cstring>
#include "DHT11NonblockingProcess2.h"
#include "main.h"
#include "DonkosInternal.h"
#include "StringConverter.h"
#include "dwt_delay.h"

volatile uint32_t CUBE_DHT_TEMP = 0;
volatile uint32_t CUBE_DHT_HUM = 0;

namespace {
    constexpr uint32_t INIT_MAX_US_LOW = 90;
    constexpr uint32_t INIT_MAX_US_HIGH = 95;
    constexpr uint32_t MAX_US_WAIT_BIT = 90;
    constexpr uint32_t HIGH_BIT_MIN_US = 60;
    constexpr uint32_t HIGH_BIT_MAX_US = 95;
    constexpr uint32_t BITS_TO_RECEIVE = 40;
}

DHT11NonblockingProcess2::DHT11NonblockingProcess2() : state{DHT_STATE::RESTART}, data_received{}, bits_received{0},
                                                       cycles{}, clearDisplay{false} {
}

void DHT11NonblockingProcess2::Main() {

    while (true) {
        if (state == DHT_STATE::RESTART) {
            restart_state();
        }
        if (state == DHT_STATE::IDLE) {
            idle_state();
        }
        if (state == DHT_STATE::PROCESS_DATA) {
            process_data_state();
        }
        if (state == DHT_STATE::COMM_ERROR) {
            comm_error_state();
        }
    }
}


void DHT11NonblockingProcess2::InterruptReceived() {
    uint32_t curr_cycles = DWT_GetTick();
    uint32_t diff_time = (curr_cycles - cycles) / 80;

    HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_SET);

    if (state == DHT_STATE::COMM_INITIALIZED_WAIT) {
        state = DHT_STATE::COMM_INITIALIZED_WAIT_FOR_DEVICE;
    } else if (state == DHT_STATE::COMM_INITIALIZED_WAIT_FOR_DEVICE) {
        state = DHT_STATE::COMM_INITIALIZED_WAIT_FOR_DEVICE_LOW;
    } else if (state == DHT_STATE::COMM_INITIALIZED_WAIT_FOR_DEVICE_LOW) {
        if (diff_time <= INIT_MAX_US_LOW) {
            state = DHT_STATE::COMM_INITIALIZED_WAIT_FOR_DEVICE_HIGH;
        } else {
            state = DHT_STATE::COMM_ERROR;
        }
    } else if (state == DHT_STATE::COMM_INITIALIZED_WAIT_FOR_DEVICE_HIGH) {
        if (diff_time <= INIT_MAX_US_HIGH) {
            state = DHT_STATE::WAIT_FOR_BIT;
        } else {
            state = DHT_STATE::COMM_ERROR;
        }
    } else if (state == DHT_STATE::WAIT_FOR_BIT) {
        if (diff_time <= MAX_US_WAIT_BIT) {
            state = DHT_STATE::READY_FOR_BIT;
        } else {
            state = DHT_STATE::COMM_ERROR;
        }
    } else if (state == DHT_STATE::READY_FOR_BIT) {
        auto index = bits_received / 8;
        auto shift = bits_received % 8;

        if (diff_time > HIGH_BIT_MIN_US && diff_time <= HIGH_BIT_MAX_US) {
            data_received[index] |= (1 << (7 - shift));
        }

        state = DHT_STATE::WAIT_FOR_BIT;
        bits_received++;

        if (bits_received >= BITS_TO_RECEIVE) {
            state = DHT_STATE::COMM_END;
        }
    } else if (state == DHT_STATE::COMM_END) {
        state = DHT_STATE::PROCESS_DATA;
    }

    cycles = curr_cycles;
    HAL_GPIO_WritePin(DEBUG_GPIO_Port, DEBUG_Pin, GPIO_PIN_RESET);
}

void DHT11NonblockingProcess2::comm_error_state() {
    Donkos_ClearDisplay();
    Donkos_Display(2, 2, "Fehler :(");
    Donkos_Display(2, 3, "");
    clearDisplay = true;
    state = DHT_STATE::RESTART;
}

void DHT11NonblockingProcess2::process_data_state() {
    uint32_t hum1 = data_received[0];
    uint32_t hum2 = data_received[1];
    uint32_t temp1 = data_received[2];
    uint32_t temp2 = data_received[3];
    uint32_t check = data_received[4];

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

        if (clearDisplay) {
            Donkos_ClearDisplay();
            clearDisplay = false;
        }
        Donkos_Display(2, 2, &output_temperature_string[0]);
        Donkos_Display(2, 3, &output_humidity_string[0]);

#ifdef Debug
        CUBE_DHT_TEMP = temp1;
        CUBE_DHT_HUM = hum1;
#endif
        state = DHT_STATE::RESTART;
    } else {
        state = DHT_STATE::COMM_ERROR;
    }
}

void DHT11NonblockingProcess2::idle_state() {
    state = DHT_STATE::COMM_INITIALIZED;
    HAL_GPIO_WritePin(DHT_DATA_GPIO_Port, DHT_DATA_Pin, GPIO_PIN_RESET);
    wait(18);

    state = DHT_STATE::COMM_INITIALIZED_WAIT;
    HAL_GPIO_WritePin(DHT_DATA_GPIO_Port, DHT_DATA_Pin, GPIO_PIN_SET);
}

void DHT11NonblockingProcess2::restart_state() {
    HAL_GPIO_WritePin(DHT_DATA_GPIO_Port, DHT_DATA_Pin, GPIO_PIN_SET);
    memset(&data_received[0], 0, 5);
    bits_received = 0;
    wait(1000);
    state = DHT_STATE::IDLE;
}
