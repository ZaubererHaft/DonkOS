#ifndef TEST_KEYBOARDPROCESS_H
#define TEST_KEYBOARDPROCESS_H


#include "Process.h"

class KeyboardProcess: public Process {
public:
    KeyboardProcess();

    void Main() override;

    void SetKeyPressed(uint16_t pin);

private:
    bool show;
};


#endif //TEST_KEYBOARDPROCESS_H
