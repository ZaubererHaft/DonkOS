#include "Inc/WiFiProcess.h"

#include <cstring>

#include "DonkosLogger.h"
#include "DonkosInternal.h"

WiFiProcess::WiFiProcess() : buffer{}, working_data{}, ipv4_address{}, buffer_overflow{} {
}

void WiFiProcess::Main() {
    Logger_Debug("Enabling WiFi...");
    if (enable()) {
        Logger_Debug("WiFi connected with IPv4 address: %s!", ipv4_address);

        Logger_Debug("Try to get weather data...");
        if (getWeatherData()) {
            Logger_Debug("Weather data available!");
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

void WiFiProcess::PackageReceived() {
    if (buffer.Push(working_data)) {
        if (HAL_UART_Receive_IT(&huart5, &working_data, 1) != HAL_OK) {
            Error_Handler();
        }
    } else {
        buffer_overflow = true;
    }
}

bool WiFiProcess::sendString(const char *text) {
    return HAL_UART_Transmit(&huart5, reinterpret_cast<const uint8_t *>(text), strlen(text), 100) == HAL_OK;
}


bool WiFiProcess::wait_for_min_size(int32_t size, int32_t timeout_in_10ms) {
    const int32_t wait_time_ms = 10;
    int32_t counter = 0;

    while (buffer.ReadLength() < size && counter < timeout_in_10ms) {
        wait(wait_time_ms);
        counter++;
    }

    return counter <= timeout_in_10ms && buffer.ReadLength() >= size;
}

bool WiFiProcess::doWaitFor(const char *text, int32_t size) {
    if (!wait_for_min_size(size)) {
        return false;
    }
    auto res = strncmp(reinterpret_cast<const char *>(buffer.read_ptr()), text, size);
    buffer.Skip(size);
    return res == 0;
}

#define wait_for_text(a) (doWaitFor(a, sizeof(a) - 1))

bool WiFiProcess::enable() {
    HAL_UART_Receive_IT(&huart5, &working_data, 1);

    HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin, GPIO_PIN_SET);
    wait(10);
    HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin, GPIO_PIN_RESET);
    wait(10);
    HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin, GPIO_PIN_SET);

    // expect "ready"-string after enable microprocessor
    if (!wait_for_min_size(318)) {
        return false;
    }
    if (!buffer.Skip(311)) {
        return false;
    }
    if (!wait_for_text("ready\r\n")) {
        return false;
    }

    // manual reset
    if (!sendString("AT+RST\r\n")) {
        return false;
    }
    if (!wait_for_text("AT+RST\r\r\n\r\nOK\r\n")) {
        return false;
    }

    // send AT station mode
    if (!sendString("AT+CWMODE=1\r\n")) {
        return false;
    }
    if (!wait_for_text("AT+CWMODE=1\r\r\n\r\nOK\r\n")) {
        return false;
    }

    // send Access Point and try to connect
    if (!sendString("AT+CWJAP=\"LuwinaNET-2.4\",\"\"\r\n")) {
        return false;
    }
    if (!wait_for_min_size(334, 1000)) {
        // ToDo: subtract size of AP name and PW and make them variable via a function
        return false;
    }
    if (!buffer.Skip(334 - 29)) {
        return false;
    }
    if (!wait_for_text("WIFI CONNECTED\r\nWIFI GOT IP\r\n")) {
        return false;
    }

    // read our IP address
    if (!sendString("AT+CIFSR\r\n")) {
        return false;
    }
    if (!wait_for_min_size(82)) {
        // ToDo subtract size of ip address
        return false;
    }
    if (!buffer.Skip(25)) {
        return false;
    }
    int32_t index = 0;
    char character = '\0';
    bool success = buffer.Pop(reinterpret_cast<unsigned char *>(&character));
    while (success && character != '\"' && index < sizeof(ipv4_address)) {
        ipv4_address[index] = character;
        success = buffer.Pop(reinterpret_cast<unsigned char *>(&character));
        index++;
    }
    if (index >= sizeof(ipv4_address)) {
        return false;
    }
    if (!buffer.Skip(buffer.ReadLength())) {
        return false;
    }

    // Enable multiple connections
    // if (!sendString("AT+CIPMUX=1\r\n")) {
    //     return false;
    // }
    // if (!wait_for_text("AT+CIPMUX=1\r\r\n\r\nOK\r\n")) {
    //     return false;
    // }

    return true;
}

bool WiFiProcess::getWeatherData() {
    // send AT station mode
    if (!sendString("AT+CIPSTART=\"TCP\",\"httpbin.org\",80\r\n")) {
        return false;
    }
    wait(1000);

    auto req = "AT+CIPSEND=57\r\n"
               "GET /get HTTP/1.1\r\n"
               "Host: httpbin.org\r\n"
               "Connection: close\r\n";
    if (!sendString(req)) {
        return false;
    }

    wait(1000);

    if (!sendString("AT+CIPCLOSE\r\n")) {
        return false;
    }
    return true;
}

