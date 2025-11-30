#include "Inc/WiFiProcess.h"

#include <cstring>

#include "DonkosLogger.h"
#include "../DonkosInternal.h"

WiFiProcess::WiFiProcess() : buffer{}, working_data{}, ipv4_address{} {
}

void WiFiProcess::Main() {
    Logger_Debug("[DBG] Enabling WiFi...\n");
    if (enable()) {
        Logger_Debug("[DBG] WiFi connected with IPv4 address: %s!\n", ipv4_address);
    }
    else {
        Logger_Debug("[ERR] Failed to enable WiFi!\n");
    }

    while (true) {
        Donkos_YieldProcess(this);
    }
}

void WiFiProcess::PackageReceived() {
    buffer.Push(working_data);
    HAL_UART_Receive_IT(&huart5, &working_data, 1);
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

#define wait_for_size_skip_and_compare_text(a, b, c) if (!wait_for_min_size(a)) { \
                                                        return false; \
                                                        } \
                                                    if (!buffer.Skip(b)) { \
                                                        return false; \
                                                    } \
                                                    if (!wait_for_text(c)) { \
                                                        return false; \
                                                    }

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
    if (!sendString("AT+CWJAP=\"\",\"\"\r\n")) {
        return false;
    }
    if (!wait_for_min_size(334, 1000)) { // ToDo: subtract size of AP name and PW and make them variable via a function
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
        return false;
    }
    if (!buffer.Skip(25)) {
        return false;
    }
    int32_t index = 0;
    char character = '\0';
    bool success = buffer.Pop(reinterpret_cast<unsigned char *>(&character));
    while (success && character != '\"') {
        ipv4_address[index] = character;
        success = buffer.Pop(reinterpret_cast<unsigned char *>(&character));
        index++;
    }
    return success;
}

