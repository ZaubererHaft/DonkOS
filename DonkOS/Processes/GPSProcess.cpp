#include "GPSProcess.h"

#include <cstring>

#include "DonkosInternal.h"

GPSProcess::GPSProcess(BaseDisplay *display) {
}


uint8_t data[512] = {0};

void GPSProcess::Main() {

    while (true) {
        std::memset(data, 0, sizeof(data));

        while (HAL_UART_Receive(&huart, (uint8_t *)data, sizeof(data), 100) == HAL_OK) {
            int i = 0;
        }

        wait(1000);
    }
}

void GPSProcess::SetHandle(const UART_HandleTypeDef &handle)  {
    huart = handle;
}
