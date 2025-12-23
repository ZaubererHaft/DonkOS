#ifndef DONKOS_ATHTTPCLIENT_H
#define DONKOS_ATHTTPCLIENT_H

#include "RingBuffer.h"

enum class ATResponseCode {
    Okay,
    TransmitFailed,
    BufferInsufficient,
    CouldNotConnectToAccessPoint,
    ModuleNotReady,
    TimeoutWaitingForOkay,
    RingBufferFull,
    CouldNotInitTransmission,
    InvalidResponse
};

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
    static constexpr int32_t RINGBUFFER_SIZE = 1024;

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
