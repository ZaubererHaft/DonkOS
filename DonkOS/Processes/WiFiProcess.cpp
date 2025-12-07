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
    auto match = strequal(text, size);
    return match;
}


bool WiFiProcess::strequal(const char *str, int32_t size) {
    return strncmp(reinterpret_cast<const char *>(buffer.read_ptr()), str, size) == 0;
}

bool WiFiProcess::do_wait_for_text_end(const char *str, int32_t size) {
    // Step 1: wait for buffer size to become at least test size for safe compare
    if (wait_for_min_size(size)) {
        int32_t counter = 0;

        // Step 2: wait until end of buffer matches the text (with new timeout)
        bool match;
        do {
            match = strncmp(reinterpret_cast<const char *>(&buffer.read_ptr()[buffer.ReadLength() - size]), str,
                            size) == 0;
            counter++;
            wait(10);
        } while (!match && counter < 1000);

        return match;
    }

    return false;
}

#define wait_for_text_end(a) (do_wait_for_text_end(a, sizeof(a) - 1))

#define wait_for_text(a) (doWaitFor(a, sizeof(a) - 1))

#define strequal(a) (strequal(a, sizeof(a) - 1))

bool WiFiProcess::enable() {
    HAL_UART_Receive_IT(&huart5, &working_data, 1);

    HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin, GPIO_PIN_SET);
    wait(100);
    HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin, GPIO_PIN_RESET);
    wait(100);
    HAL_GPIO_WritePin(WIFI_ENABLE_GPIO_Port, WIFI_ENABLE_Pin, GPIO_PIN_SET);

    // manual reset
    if (!sendString("AT+RST\r\n")) {
        return false;
    }

    // try to connect (skip if already connected)
    if (!wait_for_text_end("WIFI GOT IP\r\n")) {
        if (wait_for_text_end("ready\r\n")) {
            // send Access Point and try to connect
            if (!sendString("AT+CWJAP=\"LuwinaNET-2.4\",\"\"\r\n")) {
                return false;
            }
            if (!wait_for_text_end("WIFI CONNECTED\r\nWIFI GOT IP\r\n")) {
                return false;
            }
        } else {
            return false;
        }
    }
    buffer.Skip(buffer.ReadLength());

    // log firmware version
    if (!sendString("AT+GMR\r\n")) {
        return false;
    }
    if (!wait_for_text_end("OK\r\n")) {
        return false;
    }
    Logger_Debug("Firmware ESP %s", buffer.read_ptr());
    buffer.Skip(buffer.ReadLength());

    // send AT station mode
    if (!sendString("AT+CWMODE=1\r\n")) {
        return false;
    }
    if (!wait_for_text_end("OK\r\n")) {
        return false;
    }
    buffer.Skip(buffer.ReadLength());

    // read our IP address
    if (!sendString("AT+CIFSR\r\n")) {
        return false;
    }
    if (!wait_for_text_end("OK\r\n")) {
        return false;
    }
    buffer.Skip(buffer.ReadLength());

    int32_t index = 0;
    char character = '\0';
    bool success = buffer.Pop(reinterpret_cast<unsigned char *>(&character));
    while (success && character != '\"' && index < sizeof(ipv4_address)) {
        if ((character >= '0' && character <= '9') || character == '.') {
            ipv4_address[index] = character;
            success = buffer.Pop(reinterpret_cast<unsigned char *>(&character));
            index++;
        } else {
            return false;
        }
    }
    if (index >= sizeof(ipv4_address)) {
        return false;
    }


    //Disable multiple connections
    if (!sendString("AT+CIPMUX=0\r\n")) {
        return false;
    }
    if (!wait_for_text_end("OK\r\n")) {
        return false;
    }
    buffer.Skip(buffer.ReadLength());

    return true;
}

bool WiFiProcess::getWeatherData() {
    // send AT station mode
    if (!sendString("AT+CIPSTART=\"TCP\",\"httpbin.org\",80\r\n")) {
        return false;
    }
    if (!wait_for_text_end("OK\r\n")) {
        return false;
    }
    buffer.Skip(buffer.ReadLength());

    if (!sendString("AT+CIPSEND=38\r\n")) {
        return false;
    }
    wait_for_text_end("> ");
    if (!sendString("GET /get HTTP/1.1\r\nHost: httpbin.org\r\n\r\n")) {
        return false;
    }

    wait(500);
    if (!sendString("AT+CIPCLOSE\r\n")) {
        return false;
    }

    return true;
}

