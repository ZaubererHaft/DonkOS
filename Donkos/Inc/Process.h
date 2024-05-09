#ifndef TEST_PROCESS_H
#define TEST_PROCESS_H

#include <cstdint>

class Process {
public:
    explicit Process(uint32_t pid);

    virtual void Execute();

    uint32_t GetPid() const;

private:
    uint32_t pid;
};


#endif //TEST_PROCESS_H
