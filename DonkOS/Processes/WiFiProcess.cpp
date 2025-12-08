#include <cstring>

#include "WiFiProcess.h"
#include "DonkosLogger.h"
#include "DonkosInternal.h"


#define wait_for_text_end(a) (do_wait_for_text_end(a, sizeof(a) - 1))

#define sendString(text) (HAL_UART_Transmit(&huart5, reinterpret_cast<const uint8_t *>(text), sizeof(text) - 1, 100) == HAL_OK)

WiFiProcess::WiFiProcess() : buffer{}, working_data{}, buffer_overflow{} {
}

void WiFiProcess::Main() {
    Logger_Debug("Enabling WiFi...");
    if (enable()) {
        Logger_Debug("WiFi connected!");

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
    buffer.SkipReadLength();

    // log firmware version
    if (!sendString("AT+GMR\r\n")) {
        return false;
    }
    if (!wait_for_okay()) {
        return false;
    }
    Logger_Debug("Firmware ESP %s", buffer.read_ptr());
    buffer.SkipReadLength();

    // send AT station mode
    if (!sendString("AT+CWMODE=1\r\n")) {
        return false;
    }
    if (!wait_for_okay_and_skip()) {
        return false;
    }

    // read our IP address
    if (!sendString("AT+CIFSR\r\n")) {
        return false;
    }
    if (!wait_for_text_end("OK\r\n")) {
        return false;
    }
    Logger_Debug("Connection information: %s", buffer.read_ptr());
    buffer.SkipReadLength();


    //Disable multiple connections
    if (!sendString("AT+CIPMUX=0\r\n")) {
        return false;
    }
    if (!wait_for_okay_and_skip()) {
        return false;
    }

    return true;
}

bool WiFiProcess::getWeatherData() {
    // start connection to server
    if (!sendString("AT+CIPSTART=\"TCP\",\"httpbin.org\",80\r\n")) {
        return false;
    }
    if (!wait_for_okay_and_skip()) {
        return false;
    }

    // Announce transmission of of bytes
    if (!sendString("AT+CIPSEND=59\r\n")) {
        return false;
    }
    if (!wait_for_text_end("> ")) {
        return false;
    }
    buffer.SkipReadLength();

    // Send request
    const char httpGetRequest[] =
            "GET /get HTTP/1.1\r\n"
            "Host: httpbin.org\r\n"
            "Connection: close\r\n"
            "\r\n";
    if (!sendString(httpGetRequest)) {
        return false;
    }
    wait_for_text_end("CLOSED\r\n");
    buffer.SkipReadLength();

    return true;
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


bool WiFiProcess::wait_for_min_size(int32_t size) {
    int32_t counter = 0;
    while (buffer.ReadLength() < size && counter < DEFAULT_TIMEOUT_IN_10MS) {
        wait(TEN_MS);
        counter++;
    }
    return counter <= DEFAULT_TIMEOUT_IN_10MS && buffer.ReadLength() >= size;
}


bool WiFiProcess::do_wait_for_text_end(const char *str, int32_t size) {
    // Step 1: wait for buffer size to become at least test size for safe comparisons
    if (wait_for_min_size(size)) {
        int32_t counter = 0;

        // Step 2: wait until end of buffer matches the text (with new timeout)
        bool match;
        do {
            match = strncmp(&buffer.read_ptr()[buffer.ReadLength() - size], str, size) == 0;
            counter++;
            wait(TEN_MS);
        } while (!match && counter < DEFAULT_TIMEOUT_IN_10MS);

        return match;
    }

    return false;
}

bool WiFiProcess::wait_for_okay() {
    return wait_for_text_end("OK\r\n");
}

bool WiFiProcess::wait_for_okay_and_skip() {
    if (!wait_for_text_end("OK\r\n")) {
        return false;
    }
    return buffer.Skip(buffer.ReadLength());
}


