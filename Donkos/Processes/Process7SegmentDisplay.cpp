#include "Process7SegmentDisplay.h"


Process7SegmentDisplay::Process7SegmentDisplay() : num{0},
                                                   mapping{0b01111110U,
                                       0b01001000U,
                                       0b00111101U,
                                       0b01101101U,
                                       0b01001011U,
                                       0b01100111U,
                                       0b11110111U,
                                       0b01001100U,
                                       0b01111111U,
                                       0b11101111U} {
    //Q0 <-> G(10)
    //Q1 <-> F(9)
    //Q2 <-> A(7)
    //Q3 <-> B(6)
    //Q4 <-> E(1)
    //Q5 <-> D(2)
    //Q6 <-> C(4)
    //Q7 <-> Dot(5)

    //  ---a---
    //  |     |
    //  f     b
    //  |     |
    //  ---g---
    //  |     |
    //  e     c
    //  |     |
    //  ---d---
}


void Process7SegmentDisplay::Main() {

    while (1) {
        Write(mapping[num]);
        wait(1000);
        num = (num + 1) % 10;
    }
}

void Process7SegmentDisplay::Write(uint8_t number) {

    WritePin(pinSTCPStorageClock, GPIO_PIN_RESET);
    for (int8_t i = 7; i >= 0; --i) {
        WritePin(pinSHCPShiftClock, GPIO_PIN_RESET);
        uint8_t bit = (number & (1 << i)) >> i;
        WritePin(pinDSSER, static_cast<GPIO_PinState>(!bit));
        WritePin(pinSHCPShiftClock, GPIO_PIN_SET);
    }
    WritePin(pinSTCPStorageClock, GPIO_PIN_SET);
}


void Process7SegmentDisplay::WritePin(uint32_t pin, GPIO_PinState state) {
    HAL_GPIO_WritePin(GPIOA, pin, state); // white jumper (clock)
}



