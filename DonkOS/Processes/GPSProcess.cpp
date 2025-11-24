#include "GPSProcess.h"

#include <cstring>
#include <iterator>

#include "DonkosInternal.h"
#include "DonkosLogger.h"

extern UART_HandleTypeDef huart4;

GPSProcess::GPSProcess(BaseDisplay *display) : ReceivedSize{}, restart{true}, buffer{} {
}

bool GPSProcess::RestartCommunication() {
    std::memset(buffer, '\0', buffer_size);

    ReceivedSize = 0;

    HAL_StatusTypeDef status;
    int32_t counter = 20;

    do {
        status = HAL_UARTEx_ReceiveToIdle_IT(&huart4, buffer, buffer_size);

        if (status != HAL_OK) {
            wait(10);
        }
        counter--;
    } while (status != HAL_OK && counter > 0);

    return status == HAL_OK;
}


void GPSProcess::Main() {
    if (restart) {
        if (!RestartCommunication()) {
            Logger_Debug("[ERR] Start GPS comm failed!\n");
        }
        restart = false;
    }

    if (ReceivedSize > 0) {
        Logger_Debug("[DBG] GPS data received:\n%s\n", reinterpret_cast<char *>(buffer));
        if (!RestartCommunication()) {
            Logger_Debug("[ERR] IDLE UART could not be restarted :/!\n");
        }
        restart = true;
    }
}


void GPSProcess::UartReceived(uint16_t size) {
    ReceivedSize = size;
}

