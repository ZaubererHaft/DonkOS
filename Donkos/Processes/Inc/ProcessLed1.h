//
// Created by ludwig on 4/23/24.
//

#ifndef TEST_PROCESSLED1_H
#define TEST_PROCESSLED1_H


#include "Process.h"

class ProcessLed1 : public  Process{
public:
    explicit ProcessLed1(uint32_t pid);

    void Execute() override;
};


#endif //TEST_PROCESSLED1_H
