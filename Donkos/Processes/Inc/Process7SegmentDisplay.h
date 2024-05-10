#ifndef TEST_PROCESS7SEGMENTDISPLAY_H
#define TEST_PROCESS7SEGMENTDISPLAY_H

#include "Process.h"
#include "main.h"


class Process7SegmentDisplay : public Process {
public:
    Process7SegmentDisplay();

    void Main() override;

    void WritePin(uint32_t pin, GPIO_PinState state);

    void Write(uint8_t number);

private:
    static constexpr uint32_t pinSHCPShiftClock = KEYBOARD_S0_Pin;   // white jumper (clock)
    static constexpr uint32_t pinSTCPStorageClock = KEYBOARD_S1_Pin; // gray jumper (latch)
    static constexpr uint32_t pinDSSER = KEYBOARD_S2_Pin;  // purple jumper (data)

    uint8_t num;
    uint8_t mapping[10];
};


#endif //TEST_PROCESS7SEGMENTDISPLAY_H
