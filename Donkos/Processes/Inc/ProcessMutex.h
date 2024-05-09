#ifndef TEST_PROCESSMUTEX_H
#define TEST_PROCESSMUTEX_H


#include "Process.h"

class ProcessMutex : public Process{
public:
    explicit ProcessMutex(uint32_t pid);

    void Execute() override;
};


#endif //TEST_PROCESSMUTEX_H
