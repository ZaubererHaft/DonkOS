#include "GPSProcess.h"

#include <cstring>
#include <iterator>

#include "DonkosInternal.h"
#include "DonkosLogger.h"

extern UART_HandleTypeDef huart4;

namespace {
    constexpr int32_t buffer_size = 82 * 12;
    uint8_t buffer[buffer_size];
    int32_t ReceivedSize = 0;
}

GPSProcess::GPSProcess(BaseDisplay *display) {
}

bool GPSProcess::restart() {
    Logger_Debug("[DBG] Try restarting IDLE UART...\n");

    std::memset(buffer, '\0', buffer_size);

    ReceivedSize = 0;

    HAL_StatusTypeDef status;
    int32_t counter = 20;

    do {
        status = HAL_UARTEx_ReceiveToIdle_IT(&huart4, buffer, buffer_size);

        if (status != HAL_OK) {
            Logger_Debug("[WRN] Failed restarting IDLE UART, try another time\n");
            wait(10);
        }
        counter--;

    }
    while (status != HAL_OK && counter > 0);

    return status == HAL_OK;
}


void GPSProcess::Main() {
    restart();

    while (true) {
        if (ReceivedSize > 0) {
            Logger_Debug("[DBG] GPS data received:\n%s\n", reinterpret_cast<char *>(buffer));
            if (!restart()) {
                Logger_Debug("[ERR] IDLE UART could not be restarted :/!\n");
            }
        }

        wait(10);
    }
}

void GPSProcess::SetHandle(const UART_HandleTypeDef &handle) {
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    ReceivedSize = Size;
}
