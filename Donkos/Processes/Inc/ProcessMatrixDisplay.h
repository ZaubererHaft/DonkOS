
#ifndef TEST_PROCESSMATRIXDISPLAY_H
#define TEST_PROCESSMATRIXDISPLAY_H

#include "Process.h"
#include "main.h"

typedef enum {
    NUM_0 = 0x00,
    NUM_1 = 0x01,
    NUM_2 = 0x02,
    NUM_3 = 0x03,
    NUM_4 = 0x04,
    NUM_5 = 0x05,
    NUM_6 = 0x06,
    NUM_7 = 0x07,
    NUM_8 = 0x08,
    NUM_9 = 0x09,
    MINUS = 0x0A,
    LETTER_E = 0x0B,
    LETTER_H = 0x0C,
    LETTER_L = 0x0D,
    LETTER_P = 0x0E,
    BLANK = 0x0F
} MAX7219_Numeric;

typedef enum {
    DIGIT_1 = 1, DIGIT_2 = 2, DIGIT_3 = 3, DIGIT_4 = 4,
    DIGIT_5 = 5, DIGIT_6 = 6, DIGIT_7 = 7, DIGIT_8 = 8
} MAX7219_Digits;

class ProcessMatrixDisplay : public Process {
public:
    ProcessMatrixDisplay();

    void Main() override;

private:
    SPI_HandleTypeDef hspi1;
    uint8_t decodeMode;

    void PrintDigit(MAX7219_Digits position, MAX7219_Numeric numeric, bool point);

    void TurnOn();

    void TurnOff();

    void Clean();

    void SetIntensivity(uint8_t intensivity);

    void SendData(uint8_t addr, uint8_t data);

    void CS_Set();

    void CS_Reset();
};


#endif //TEST_PROCESSMATRIXDISPLAY_H
