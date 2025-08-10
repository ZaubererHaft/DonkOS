#include "Process7SegmentDisplay.h"


void clearShiftRegisters()
{
    HAL_GPIO_WritePin(NOE_GPIO_Port, NOE_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(NSRCLR_GPIO_Port, NSRCLR_Pin, GPIO_PIN_RESET);
}


Process7SegmentDisplay::Process7SegmentDisplay() : num{0},
                                                   mapping{

    //  Pins from l.t.r seen from top
    //  g-f-VCC-a-b
    //
    //  ---a---
    //  |     |
    //  f     b
    //  |     |
    //  ---g---
    //  |     |
    //  e     c
    //  |     |
    //  ---d---
    //
    //  e-d-VCC-c-Dot

    //QH QG QF QE QD QC QB QA <- read from r.t.l
    //G F D E DP C A B
   0b0111'0111,
   0b0000'0101U,
   0b1011'0011U,
   0b1010'0111U,
   0b1100'0101U,
   0b1110'0110U,
   0b1111'1110U,
   0b0000'0111U,
   0b1111'0111U,
   0b1110'1111U
} {
    clearShiftRegisters();
//Neu
    //QA <-> B
    //QB <-> A
    //QC <-> C
    //QD <-> DP
    //QE <-> E
    //QF <-> D
    //QG <-> F
    //QH <-> G
//Alt 1
//Alt 2
    //QA <-> G
    //QB <-> F
    //QC <-> A
    //QD <-> B
    //QE <-> Dot
    //QF <-> C
    //QG <-> D
    //QH <-> E
//Alt 1
    //Q0 <-> G(10)
    //Q1 <-> F(9)
    //Q2 <-> A(7)
    //Q3 <-> B(6)
    //Q4 <-> E(1)
    //Q5 <-> D(2)
    //Q6 <-> C(4)
    //Q7 <-> Dot(5)

}

void Process7SegmentDisplay::Main() {


    while (1) {
        Write(mapping[num]);
        wait(1000);
        num = (num + 1) % 10;
    }
}

void Process7SegmentDisplay::Write(uint8_t number) {

    HAL_GPIO_WritePin(RCLK_GPIO_Port, RCLK_Pin, GPIO_PIN_RESET);

    for (int8_t i = 7; i >= 0; --i) {
        uint8_t bit = (number & (1 << i)) >> i;

        HAL_GPIO_WritePin(SRCLK_GPIO_Port, SRCLK_Pin, GPIO_PIN_RESET);

        HAL_GPIO_WritePin(NOE_GPIO_Port, NOE_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(NSRCLR_GPIO_Port, NSRCLR_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(SER_GPIO_Port, SER_Pin, static_cast<GPIO_PinState>(!bit));

        HAL_GPIO_WritePin(SRCLK_GPIO_Port, SRCLK_Pin, GPIO_PIN_SET);

    }

    HAL_GPIO_WritePin(NOE_GPIO_Port, NOE_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(NSRCLR_GPIO_Port, NSRCLR_Pin, GPIO_PIN_SET);

    HAL_GPIO_WritePin(RCLK_GPIO_Port, RCLK_Pin, GPIO_PIN_SET);
}


void Process7SegmentDisplay::WritePin(uint32_t pin, GPIO_PinState state) {
    HAL_GPIO_WritePin(GPIOA, pin, state); // white jumper (clock)
}



