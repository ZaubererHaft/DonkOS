
#ifndef TEST_BUZZERPROCESS_H
#define TEST_BUZZERPROCESS_H

#include "Process.h"
#include "main.h"

class BuzzerProcess : public Process{
public:
    BuzzerProcess();

    void Init();

    void Main() override;

private:
    static constexpr uint32_t TimerClock = 4'000'000;

    TIM_HandleTypeDef htim1;

    uint32_t prescalerValueForFrequency(uint32_t frequency);
};


#endif //TEST_BUZZERPROCESS_H
