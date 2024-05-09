//
// Created by ludwig on 4/23/24.
//

#ifndef TEST_PROCESSLED2_H
#define TEST_PROCESSLED2_H


#include "Process.h"

class ProcessLed2 : public Process {
public:
    explicit ProcessLed2(uint32_t pid);

    void Execute() override;
};


#endif //TEST_PROCESSLED2_H
