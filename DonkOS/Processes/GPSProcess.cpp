#include "GPSProcess.h"

#include <cstring>
#include <iterator>

#include "DonkosInternal.h"
#include "DonkosLogger.h"
#include "StringConverter.h"

extern UART_HandleTypeDef huart4;

GPSProcess::GPSProcess(BaseDisplay *display) : ReceivedSize{}, buffer{}, lineBuffer{}, messages_buffer{},
                                               parser{}, position_received{false} {
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
    Donkos_Display(4, 2, "Suche GPS...");

    if (ReceivedSize > 0) {
        Logger_Debug("GPS data received: %s", reinterpret_cast<char *>(buffer));

        auto status = parser.Parse(reinterpret_cast<char *>(buffer), messages_buffer, 10);
        if (status == ParseResult::Okay) {
            for (auto &message: messages_buffer) {
                if (message.messageType == NMEAMessageType::GPGGA_GLOBAL_POSITIONING_FIXED_DATA) {
                    auto gpgga = message.gpgga_message;

                    float lat = static_cast<float>(gpgga.latitude.degree) + gpgga.latitude.minute / 60.0f;
                    float lon = static_cast<float>(gpgga.longitude.degree) + gpgga.longitude.minute / 60.0f;

                    //ToDo: Send lat-long to WiFi process and read weather data

                    StringConverter conv{};
                    char latbuf[12]{};
                    char longbuf[12]{};

                    conv.FloatToString(lat, latbuf, 12, {.places_after_dot = 4});
                    conv.FloatToString(lon, longbuf, 12, {.places_after_dot = 4});

                    if (lat > 0.0f && lon > 0.0f) {
                        position_received = true;
                        Donkos_Display(4, 2, "GPS verbunden");
                        Logger_Debug("Our position is: %s,%s", latbuf, longbuf);

                    }
                }
            }
        } else {
            Logger_Debug("GPS data could not be parsed, reason %s", ParseResult_ToString(status));
        }
    }


    if (!position_received) {
        wait(5000);

        Logger_Debug("No valid position available, trying to get our location...");
        if (RestartCommunication()) {
            Logger_Debug("IDLE UART successfully started!");
        } else {
            Logger_Error("IDLE UART could not be started, giving up :/!");
        }
    }
}


void GPSProcess::UartReceived(uint16_t size) {
    ReceivedSize = size;
}

