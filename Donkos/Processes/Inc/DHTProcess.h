#ifndef TEST_DHTPROCESS_H
#define TEST_DHTPROCESS_H


#include "Process.h"
#include "main.h"

class DHTProcess : public Process {
public:
    DHTProcess();

    void Main() override;

private:
    TIM_HandleTypeDef htim1;
    uint32_t pMillis, cMillis;

    void microDelay(uint16_t);

    uint8_t Start();

    uint8_t Read();

};


#endif //TEST_DHTPROCESS_H
