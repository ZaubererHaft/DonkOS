#ifndef DONKOS_ATHTTPCLIENT_H
#define DONKOS_ATHTTPCLIENT_H

#include "RingBuffer.h"


constexpr const char* ATResponseStringMap[] = {
    "Okay",
    "UART_RingBufferFull",
    "UART_CommunicationInitFailed",
    "CIFSR_TransmitFailed",
    "CIFSR_TimeoutWaitingForOkay",
    "CIFSR_UnexpectedResponse",
    "GMR_TransmitFailed",
    "GMR_TimeoutWaitingForOkay",
    "GMR_UnexpectedResponse",
    "GMR_BufferInsufficientForData",
    "RST_TransmitFailed",
    "CWMODE_TransmitFailed",
    "CWMODE_TimeoutWaitingForOkay",
    "CIPMUX_TransmitFailed",
    "CIPMUX_TimeoutWaitingForOkay",
    "CWJAP_BufferInsufficientForSettings",
    "CWJAP_TransmitFailed",
    "CWJAP_CouldNotConnectToAccessPoint",
    "CIPDNS_TransmitFailed",
    "CIPDNS_TimeoutWaitingForOkay",
    "CIPSTART_TransmitFailed",
    "CIPSTART_TimeoutWaitingForOkay",
    "CIPSEND_TransmitFailed",
    "CIPSEND_TimeoutWaitingForByteStart",
    "GET_REQUEST_BufferInsufficientForHostAndPath",
    "GET_REQUEST_TransmitFailed",
    "GET_REQUEST_TimeoutWaitingForConnectionClose",
    "GET_REQUEST_InvalidResponse",
    "GET_REQUEST_ResponseBufferInsufficient",
    "CIPSNTP_TransmitFailed",
    "CIPSNTP_TimeoutWaitingForOkay",
    "CIPSSLSIZE_TransmitFailed",
    "CIPSSLSIZE_TimeoutWaitingForOkay",
};

enum class ATResponseCode {
    Okay,
    UART_RingBufferFull,
    UART_CommunicationInitFailed,
    CIFSR_TransmitFailed,
    CIFSR_TimeoutWaitingForOkay,
    CIFSR_UnexpectedResponse,
    GMR_TransmitFailed,
    GMR_TimeoutWaitingForOkay,
    GMR_UnexpectedResponse,
    GMR_BufferInsufficientForData,
    RST_TransmitFailed,
    CWMODE_TransmitFailed,
    CWMODE_TimeoutWaitingForOkay,
    CIPMUX_TransmitFailed,
    CIPMUX_TimeoutWaitingForOkay,
    CWJAP_BufferInsufficientForSettings,
    CWJAP_TransmitFailed,
    CWJAP_CouldNotConnectToAccessPoint,
    CIPDNS_TransmitFailed,
    CIPDNS_TimeoutWaitingForOkay,
    CIPSTART_TransmitFailed,
    CIPSTART_TimeoutWaitingForOkay,
    CIPSEND_TransmitFailed,
    CIPSEND_TimeoutWaitingForByteStart,
    GET_REQUEST_BufferInsufficientForHostAndPath,
    GET_REQUEST_TransmitFailed,
    GET_REQUEST_TimeoutWaitingForConnectionClose,
    GET_REQUEST_InvalidResponse,
    GET_REQUEST_ResponseBufferInsufficient,
    CIPSNTP_TransmitFailed,
    CIPSNTP_TimeoutWaitingForOkay,
    CIPSSLSIZE_TransmitFailed,
    CIPSSLSIZE_TimeoutWaitingForOkay,
};

const char *ATResponseCode_ToString(const ATResponseCode &code);

class ATVersionInfo {
public:
    char firmware[192];
};

class ATConnectStatus {
public:
    char ipv4_address[16];
    char mac_address[18];
};

class ATWiFiConnectSettings {
public:
    const char *AccessPoint;
    const char *Password;
};

class ATHTTPRequestSettings {
public:
    bool ssl;
    const char *host;
    const char *path;
    char *response_buffer;
    int32_t response_buffer_size;
};

class AT {
public:
    AT();

    ATResponseCode Enable();

    ATResponseCode ConnectToWiFi(const ATWiFiConnectSettings &settings);

    ATResponseCode GetRequest(const ATHTTPRequestSettings &settings);

    ATResponseCode PackageReceived();

    ATResponseCode ReadConnectionState(ATConnectStatus &out_connectStatus);

    ATResponseCode ReadFirmware(ATVersionInfo &out_versionInfo);

private:
    static constexpr int32_t RINGBUFFER_SIZE = 8192;

    RingBuffer<char, RINGBUFFER_SIZE> buffer;
    uint8_t working_data;

    bool wait_for_min_size(int32_t size) const;

    bool wait_for_text_end(const char *str, int32_t size) const;

    bool wait_for_okay() const;

    bool wait_for_okay_and_skip();

    static bool sendString(const char *string, std::size_t len);

    ATResponseCode rollback_and_return(ATResponseCode code);
};

#endif //DONKOS_ATHTTPCLIENT_H
