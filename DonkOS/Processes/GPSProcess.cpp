#include "GPSProcess.h"

#include <cstring>
#include <iterator>

#include "DonkosInternal.h"
#include "DonkosLogger.h"

extern UART_HandleTypeDef huart4;

namespace {
    constexpr int32_t size_to_receive = 2048;

    uint8_t buffer[size_to_receive * 2];
    uint8_t *currBuffer = nullptr;
    int32_t ReceivedSize = 0;
    bool working = false;
}

GPSProcess::GPSProcess(BaseDisplay *display) {
}


void GPSProcess::Main() {
    currBuffer = buffer;

    // start IDLE UART. Done in a loop because we might be out of sync at this moment., thus, try it some times an wait to find a synchronization point
    HAL_StatusTypeDef status;
    int start_counter = 10;
    do {
        status = HAL_UARTEx_ReceiveToIdle_IT(&huart4, currBuffer, size_to_receive);
        Logger_Debug("[DBG] Try restarting IDLE UART\n");
        wait(100);
        start_counter--;
    } while (start_counter > 0 && status != HAL_OK);

    if (start_counter <= 0 && status != HAL_OK) {
        Logger_Debug("[ERR] Failed restarting IDLE UART\n");
        Error_Handler();
    }

    // now: handle received data if available
    while (true) {
        if (!working) {
            if (ReceivedSize <= 0) {
                Logger_Debug("[DBG] No GPS data available\n");
            } else {
                Logger_Debug("[DBG] GPS data received:\n%s\n", reinterpret_cast<char *>(currBuffer));
                ReceivedSize = 0;
            }
        }

        wait(1000);
    }
}

void GPSProcess::SetHandle(const UART_HandleTypeDef &handle) {
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
    working = true;
    ReceivedSize = Size;

    if (currBuffer == buffer) {
        currBuffer = &buffer[size_to_receive];
    } else {
        currBuffer = buffer;
    }

    if (HAL_UARTEx_ReceiveToIdle_IT(&huart4, currBuffer, size_to_receive) != HAL_OK) {
        Logger_Debug("[ERR] could not restart IDLE UART\n");
    }

    working = false;
}
