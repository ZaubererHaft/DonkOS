
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
    static constexpr uint32_t TimerClock = 60'000'000;

    TIM_HandleTypeDef htim1;

    uint32_t prescalerValueForFrequency(uint32_t frequency);

    void tone(uint32_t freq, int32_t ms);

    void noTone(int32_t ms);
};


#endif //TEST_BUZZERPROCESS_H
