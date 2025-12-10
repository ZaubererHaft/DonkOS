#ifndef DONKOS_WIFIPROCESS_H
#define DONKOS_WIFIPROCESS_H

#include "AT.h"
#include "Process.h"
#include "main.h"
#include "RingBuffer.h"


class WiFiProcess : public Process {
public:
    WiFiProcess();

    void Main() override;

    void PackageReceived();

private:
    static constexpr int32_t DEFAULT_TIMEOUT_IN_10MS = 1000;
    static constexpr int32_t TEN_MS = 10;

    char response_buffer[1024];

    bool enable();

    bool getWeatherData();

    bool parseWeatherData();

    AT at_interface;

    bool buffer_overflow;
};


#endif //DONKOS_WIFIPROCESS_H
