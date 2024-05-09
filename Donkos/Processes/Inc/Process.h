#ifndef TEST_PROCESS_H
#define TEST_PROCESS_H

#include <cstdint>

class Process {
public:
    explicit Process();

    virtual void Execute();

    uint32_t InitStack();

    void SetPid(uint32_t pid);

    uint32_t GetPid() const;

private:
    static constexpr uint32_t stackSize = 256;

    uint32_t pid;
    uint32_t stack[stackSize];
};


#endif //TEST_PROCESS_H
