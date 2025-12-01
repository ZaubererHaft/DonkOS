#include "GPSProcess.h"

#include <cstring>
#include <iterator>

#include "DonkosInternal.h"
#include "DonkosLogger.h"
#include "StringConverter.h"

extern UART_HandleTypeDef huart4;

GPSProcess::GPSProcess(BaseDisplay *display) : ReceivedSize{}, restart{true}, buffer{}, lineBuffer{}, messages_buffer{},
                                               parser{} {
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
        Logger_Debug("[DBG] GPS data received: %s\n", reinterpret_cast<char *>(buffer));

        auto status = parser.Parse(reinterpret_cast<char *>(buffer), messages_buffer, 10);
        if (status == ParseResult::Okay) {
            for (auto &message: messages_buffer) {
                if (message.messageType == NMEAMessageType::GPGGA_GLOBAL_POSITIONING_FIXED_DATA) {
                    auto gpgga = message.gpgga_message;

                    float lat = gpgga.latitude.degree + gpgga.latitude.minute / 60.0;
                    float lon = gpgga.longitude.degree + gpgga.longitude.minute / 60.0;

                    StringConverter conv{};
                    char latbuf[12]{};
                    char longbuf[12]{};

                    conv.FloatToString(lat, latbuf, 12, {.places_after_dot =  4});
                    conv.FloatToString(lon, longbuf, 12, {.places_after_dot = 4});

                    Logger_Debug("[DBG] Our position is: %s,%s \n", latbuf, longbuf);
                }
            }
        }

        if (!RestartCommunication()) {
            Logger_Debug("[ERR] IDLE UART could not be restarted :/!\n");
        }
        restart = true;
    }
}


void GPSProcess::UartReceived(uint16_t size) {
    ReceivedSize = size;
}

