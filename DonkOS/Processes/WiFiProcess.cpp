#include <cstring>

#include "WiFiProcess.h"
#include "DonkosLogger.h"
#include "DonkosInternal.h"


WiFiProcess::WiFiProcess() : at_interface{} {
}

void WiFiProcess::Main() {
    Logger_Debug("Enabling WiFi...");
    if (enable()) {
        Logger_Debug("WiFi connected!");

        Logger_Debug("Try to get weather data...");
        if (getWeatherData()) {
            Logger_Debug("Weather data available, parsing data...");
            if (parseWeatherData()) {
                Logger_Debug("Weather data parsed successfully!");
            } else {
                Logger_Error("Failed to parse weather data!");
            }
        } else {
            Logger_Error("Failed to get weather data!");
        }
    } else {
        Logger_Error("Failed to enable WiFi!");
    }

    while (true) {
        if (buffer_overflow) {
            Logger_Error("WiFi buffer overflow :(!");
            Donkos_EndProcess(this);
        } else {
            Donkos_YieldProcess(this);
        }
    }
}

bool WiFiProcess::enable() {
    auto status = at_interface.EnableAndStartWiFiConnection({
        "LuwinaNET-2.4",
        ""
    });

    return status == ATResponseCode::Okay;
}

bool WiFiProcess::getWeatherData() {
    return at_interface.GetRequest(
               {
                   .host = "httpbin.org",
                   .path = "get",
                   .response_buffer = response_buffer,
                   .response_buffer_size = sizeof(response_buffer)
               }
           ) == ATResponseCode::Okay;
}

bool WiFiProcess::parseWeatherData() {
    return true;
}

void WiFiProcess::PackageReceived() {
    if (at_interface.PackageReceived() != ATResponseCode::Okay) {
        buffer_overflow = true;
    }
}
