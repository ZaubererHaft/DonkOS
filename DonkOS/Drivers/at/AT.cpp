#include "AT.h"

#include <algorithm>

#include "ATHAL.h"

#include <cstdio>
#include <cstring>

#include "DonkosInternal.h"

namespace {
    constexpr int32_t DEFAULT_TIMEOUT_IN_10MS = 1000;
    constexpr int32_t TEN_MS = 10;

    constexpr auto *AT_CIFSR = "AT+CIFSR\r\n";
    constexpr auto AT_CIFSR_SIZE = sizeof("AT+CIFSR\r\n") - 1;

    constexpr auto *AT_GMR = "AT+GMR\r\n";
    constexpr auto AT_GMR_SIZE = sizeof("AT+GMR\r\n") - 1;

    constexpr auto *AT_RST = "AT+RST\r\n";
    constexpr auto AT_RST_SIZE = sizeof("AT+RST\r\n") - 1;

    constexpr auto *AT_OKAY = "OK\r\n";
    constexpr auto AT_OKAY_SIZE = sizeof("OK\r\n") - 1;

    constexpr auto AT_CWJAP_PATTERN = "AT+CWJAP=\"%s\",\"%s\"\r\n";
    constexpr auto AT_CWJAP_OVERHEAD = sizeof("AT+CWJAP=\"\",\"\"\r\n") - 1;

    constexpr auto *AT_WIFI_GOT_IP = "WIFI GOT IP\r\n";
    constexpr auto AT_WIFI_GOT_IP_SIZE = sizeof("WIFI GOT IP\r\n") - 1;

    constexpr auto *AT_READY = "ready\r\n";
    constexpr auto AT_READY_SIZE = sizeof("ready\r\n") - 1;

    constexpr auto AT_CWMODE_STATION = "AT+CWMODE=1\r\n";
    constexpr auto AT_CWMODE_STATION_SIZE = sizeof("AT+CWMODE=1\r\n") - 1;

    constexpr auto AT_CIPMUX_SINGLE = "AT+CIPMUX=0\r\n";
    constexpr auto AT_CIPMUX_SINGLE_SIZE = sizeof("AT+CIPMUX=0\r\n") - 1;

    constexpr auto *AT_CLOSED = "CLOSED\r\n";
    constexpr auto AT_CLOSED_SIZE = sizeof("CLOSED\r\n") - 1;

    constexpr auto *AT_CIPSTART_TCP_PATTERN = "AT+CIPSTART=\"TCP\",\"%s\",80\r\n";
    constexpr auto AT_CIPSTART_TCP_OVERHEAD = sizeof("AT+CIPSTART=\"TCP\",\"\",80\r\n") - 1;

    constexpr auto *AT_GET_REQUEST_PATTERN =
            "GET /%s HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Connection: close\r\n"
            "\r\n";
    constexpr auto AT_GET_REQUEST_OVERHEAD = sizeof("GET /get HTTP/1.1\r\n"
                                                 "Host: \r\n"
                                                 "Connection: close\r\n"
                                                 "\r\n") - 1;

    constexpr auto *AT_CIPSEND_PATTERN = "AT+CIPSEND=%u\r\n";
    constexpr auto AT_CIPSEND_OVERHEAD = sizeof("AT+CIPSEND=\r\n") - 1;
}

AT::AT() : buffer{}, working_data{}, connect_status{} {
}

ATResponseCode AT::ReadConnectionState(ATConnectStatus &out_connectStatus) {
    // read our IP address
    if (!sendString(AT_CIFSR, AT_CIFSR_SIZE)) {
        return ATResponseCode::TransmitFailed;
    }
    if (!wait_for_okay()) {
        return ATResponseCode::TimeoutWaitingForOkay;
    }

    //ToDo: parse to class
    buffer.SkipReadLength();
    return ATResponseCode::Okay;
}

ATResponseCode AT::ReadFirmware(ATVersionInfo &out_versionInfo) {
    // log firmware version
    if (!sendString(AT_GMR, AT_GMR_SIZE)) {
        return ATResponseCode::TransmitFailed;
    }
    if (!wait_for_okay()) {
        return ATResponseCode::TimeoutWaitingForOkay;
    }

    if (buffer.ReadLength() < sizeof(out_versionInfo.firmware)) {
        std::memset(out_versionInfo.firmware, 0, sizeof(out_versionInfo.firmware));
        if (buffer.CopyFromHeadAndSkip(reinterpret_cast<uint8_t *>(out_versionInfo.firmware),
                                       sizeof(out_versionInfo.firmware))) {
            return ATResponseCode::Okay;
        }
        return ATResponseCode::BufferInsufficient;
    }

    return ATResponseCode::Okay;
}

ATResponseCode AT::EnableAndStartWiFiConnection(const ATWiFiConnectSettings &settings) {
    if (!ATHAL_StartUARTReceiveIT(&working_data)) {
        return ATResponseCode::CouldNotInitTransmission;
    }

    ATHAL_EnableChip();
    ATHAL_Wait(100);
    ATHAL_DisableChip();
    ATHAL_Wait(100);
    ATHAL_EnableChip();

    // manual reset
    if (!sendString(AT_RST, AT_RST_SIZE)) {
        return ATResponseCode::TransmitFailed;
    }

    // try to connect (skip if already connected)
    if (!wait_for_text_end(AT_WIFI_GOT_IP, AT_WIFI_GOT_IP_SIZE)) {
        if (wait_for_text_end(AT_READY, AT_READY_SIZE)) {
            // send Access Point and try to connect
            char command[50 + AT_CWJAP_OVERHEAD]{};

            if (strnlen(settings.AccessPoint, 51) + strnlen(settings.Password, 51) + AT_CWJAP_OVERHEAD <= sizeof(
                    command)) {
                sprintf(command, AT_CWJAP_PATTERN, settings.AccessPoint, settings.Password);
            } else {
                return ATResponseCode::BufferInsufficient;
            }

            if (!wait_for_text_end(AT_WIFI_GOT_IP, AT_WIFI_GOT_IP_SIZE)) {
                return ATResponseCode::CouldNotConnectToWiFi;
            }
        } else {
            return ATResponseCode::ModuleNotReady;
        }
    }
    buffer.SkipReadLength();

    // send AT in station mode
    if (!sendString(AT_CWMODE_STATION, AT_CWMODE_STATION_SIZE)) {
        return ATResponseCode::TransmitFailed;
    }
    if (!wait_for_okay_and_skip()) {
        return ATResponseCode::TimeoutWaitingForOkay;
    }

    //Disable multiple connections
    if (!sendString(AT_CIPMUX_SINGLE, AT_CIPMUX_SINGLE_SIZE)) {
        return ATResponseCode::TransmitFailed;
    }
    if (!wait_for_okay_and_skip()) {
        return ATResponseCode::TimeoutWaitingForOkay;
    }

    return ATResponseCode::Okay;
}

ATResponseCode AT::GetRequest(const ATHTTPRequestSettings &settings) {
    char command[40 + AT_GET_REQUEST_OVERHEAD]{};
    auto host_size = strnlen(settings.host, 41);
    auto path_size = strnlen(settings.path, 41);

    //Make sure buffer is large enough for the following requests (the other sprintf calls are safe then because the GET_REQUEST has the most overhead)
    if (host_size + path_size + AT_GET_REQUEST_OVERHEAD > sizeof(command)) {
        return ATResponseCode::BufferInsufficient;
    }

    // start connection to server
    sprintf(command, AT_CIPSTART_TCP_PATTERN, settings.host);
    if (!sendString(command, strnlen(command, sizeof(command)))) {
        return ATResponseCode::TransmitFailed;
    }
    if (!wait_for_okay_and_skip()) {
        return ATResponseCode::TimeoutWaitingForOkay;
    }

    // Announce transmission of bytes
    auto bytes_to_send = host_size + AT_GET_REQUEST_OVERHEAD;
    std::memset(command, 0, sizeof(command));
    sprintf(command, AT_CIPSEND_PATTERN, bytes_to_send);
    if (!sendString(command, strnlen(command, sizeof(command)))) {
        return ATResponseCode::TransmitFailed;
    }
    if (!wait_for_text_end("> ", 2)) {
        return ATResponseCode::TimeoutWaitingForOkay;
    }
    buffer.SkipReadLength();

    // Send request & copy response
    std::memset(command, 0, sizeof(command));
    sprintf(command, AT_GET_REQUEST_PATTERN, settings.path, settings.host);
    if (!sendString(command, strnlen(command, sizeof(command)))) {
        return ATResponseCode::TransmitFailed;
    }
    if (!wait_for_text_end(AT_CLOSED, AT_CLOSED_SIZE)) {
        return ATResponseCode::TimeoutWaitingForOkay;
    }

    // Skip until IDP start was found
    char character;
    bool result;
    do {
        result = buffer.Pop(&character);
    } while (result && character != '+');
    // SKIP ' IPD,XXX: ', too
    result &= buffer.Skip(8);
    if (!result) {
        return ATResponseCode::IllegalResponse;
    }

    // FINALLY: Copy to buffer
    if (!buffer.CopyFromHead(reinterpret_cast<uint8_t *>(settings.response_buffer),
                            settings.response_buffer_size, buffer.ReadLength() - AT_CLOSED_SIZE)) {
        return ATResponseCode::BufferInsufficient;
    }
    buffer.SkipReadLength();

    return ATResponseCode::Okay;
}

ATResponseCode AT::PackageReceived() {
    if (buffer.Push(working_data)) {
        if (ATHAL_StartUARTReceiveIT(&working_data)) {
            return ATResponseCode::Okay;
        }
        return ATResponseCode::CouldNotInitTransmission;
    }

    return ATResponseCode::RingBufferFull;
}


bool AT::wait_for_min_size(const int32_t size) const {
    int32_t counter = 0;
    while (buffer.ReadLength() < size && counter < DEFAULT_TIMEOUT_IN_10MS) {
        ATHAL_Wait(TEN_MS);
        counter++;
    }
    return counter <= DEFAULT_TIMEOUT_IN_10MS && buffer.ReadLength() >= size;
}


bool AT::wait_for_text_end(const char *str, const int32_t size) const {
    if (size > 20) {
        return false;
    }


    // Step 1: wait for buffer size to become at least test size for safe comparisons
    if (wait_for_min_size(size)) {
        int32_t counter = 0;

        // Step 2: wait until end of buffer matches the text (with new timeout)
        bool match;
        do {
            char copy_buffer[21]{};
            buffer.Copy(reinterpret_cast<uint8_t *>(copy_buffer), sizeof(copy_buffer), buffer.Tail() - size, size);
            match = strncmp(copy_buffer, str, size) == 0;
            counter++;
            ATHAL_Wait(TEN_MS);
        } while (!match && counter < DEFAULT_TIMEOUT_IN_10MS);

        return match;
    }

    return false;
}

bool AT::wait_for_okay() const {
    return wait_for_text_end(AT_OKAY, AT_OKAY_SIZE);
}

bool AT::wait_for_okay_and_skip() {
    if (!wait_for_text_end(AT_OKAY, AT_OKAY_SIZE)) {
        return false;
    }
    buffer.SkipReadLength();
    return true;
}

bool AT::sendString(const char *string, std::size_t len) {
    return ATHAL_Transmit(reinterpret_cast<const uint8_t *>(string), len);
}

