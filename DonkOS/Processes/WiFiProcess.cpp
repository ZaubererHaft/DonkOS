#include <cstring>

#include "WiFiProcess.h"
#include "DonkosLogger.h"
#include "DonkosInternal.h"


WiFiProcess::WiFiProcess() : response_buffer{}, at_interface{}, buffer_overflow{false} {
}

void WiFiProcess::Main() {
    Donkos_Display(4, 3, "Suche WiFi...");

    Logger_Debug("Enabling WiFi...");
    if (enableWifi()) {
        Donkos_Display(4, 3, "WiFi verbunden");
        Logger_Debug("WiFi connected!");

        ATVersionInfo info{};
        if (at_interface.ReadFirmware(info) == ATResponseCode::Okay) {
            Logger_Debug("Firmware version: %s", info.firmware);
        }

        ATConnectStatus status{};
        if (at_interface.ReadConnectionState(status) == ATResponseCode::Okay) {
            Logger_Debug("Connection status: IP-Address: %s MAC-Address: %s", status.ipv4_address, status.mac_address);
        }

        Logger_Debug("Try to get weather data...");
        if (getWeatherData()) {
            Logger_Debug("Available data: %s\n", response_buffer);
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

bool WiFiProcess::enableWifi() {
    auto ret = false;
    auto result = at_interface.Enable();
    if (result == ATResponseCode::Okay) {
        result = at_interface.ConnectToWiFi({
            "LuwinaNET-2.4",
        });
        if (result == ATResponseCode::Okay) {
            ret = true;
        }
    }

    Logger_Debug("AT module returns: %s", ATResponseCode_ToString(result));
    return ret;
}

bool WiFiProcess::getWeatherData() {
    auto status = at_interface.GetRequest(
        {
            .host = "api.open-meteo.com",
            .path = "v1/forecast?latitude=48.3&longitude=11.6&hourly=temperature_2m",
            .response_buffer = response_buffer,
            .response_buffer_size = sizeof(response_buffer)
        }
    );
    Logger_Debug("AT module returns: %s", ATResponseCode_ToString(status));
    return status == ATResponseCode::Okay;
}

bool WiFiProcess::parseWeatherData() {
    return true;
}

void WiFiProcess::PackageReceived() {
    if (at_interface.PackageReceived() != ATResponseCode::Okay) {
        buffer_overflow = true;
    }
}

