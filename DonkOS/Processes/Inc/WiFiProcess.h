#ifndef DONKOS_WIFIPROCESS_H
#define DONKOS_WIFIPROCESS_H

#include "Process.h"
#include "main.h"
#include "RingBuffer.h"

extern UART_HandleTypeDef huart5;

class WiFiProcess : public Process{

public:
    WiFiProcess();

    void Main() override;

    void PackageReceived();

private:
    static constexpr int32_t DEFAULT_TIMEOUT_IN_10MS = 100;

    bool enable();

    bool getWeatherData();

    bool sendString(const char *text);

    bool doWaitFor(const char *text, int32_t size);

    bool wait_for_min_size(int32_t size, int32_t timeout_in_10ms = DEFAULT_TIMEOUT_IN_10MS);

    bool strequal(const char * str, int32_t size);

    bool do_wait_for_text_end(const char *str, int32_t size);

    RingBuffer<uint8_t, 2048> buffer;
    uint8_t working_data;
    char ipv4_address[4 * 4 + 3 + 1]{};
    bool buffer_overflow;

};



#endif //DONKOS_WIFIPROCESS_H