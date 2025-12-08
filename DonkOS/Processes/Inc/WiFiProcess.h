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
    static constexpr int32_t DEFAULT_TIMEOUT_IN_10MS = 1000;
     static constexpr int32_t TEN_MS = 10;


    bool enable();

    bool getWeatherData();

    bool wait_for_min_size(int32_t size);

    bool do_wait_for_text_end(const char *str, int32_t size);

    bool wait_for_okay();

    bool wait_for_okay_and_skip();

    RingBuffer<char, 2048> buffer;
    uint8_t working_data;
    bool buffer_overflow;

};



#endif //DONKOS_WIFIPROCESS_H