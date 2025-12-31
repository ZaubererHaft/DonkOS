#ifndef DONKOS_ATHTTPCLIENT_H
#define DONKOS_ATHTTPCLIENT_H

#include "RingBuffer.h"
#include "ATResponseCode.h"


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

enum class ATMonth {
    January, February, March, April, May, June, July, August, September, October, November, December
};

enum class ATDayOfWeek {
    Monday, Tuesday, Wednesday, Thursday, Friday, Saturday, Sunday
};

class ATSNTPTime {
public:
    uint16_t year;
    ATMonth month;
    uint8_t day;
    ATDayOfWeek day_of_week;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
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

    ATResponseCode GetTime(ATSNTPTime &out_time);

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
