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

    uint8_t num;
    uint8_t mapping[10];
};


#endif //TEST_PROCESS7SEGMENTDISPLAY_H
