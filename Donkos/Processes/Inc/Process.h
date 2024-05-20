#ifndef TEST_PROCESS_H
#define TEST_PROCESS_H

#include <cstdint>

enum class ProcessState {
    CREATED, READY, WAITING, RUNNING, DESTROYED
};

class Process {
public:
    explicit Process();

    virtual void Main();

    uint32_t InitStack();

    void SetPid(uint32_t newPid);

    uint32_t GetPid() const;

    uint32_t GetStackPointer() const;

    void SaveContext(uint32_t *regs);

    void LoadContext(uint32_t *regs);

    ProcessState GetState() const;

    void UpdateTimer();

    void SetState(ProcessState state);

protected:
    void wait(int32_t milliseconds);

private:
    static constexpr uint32_t stackSize = 64U;

    uint32_t pid;
    uint32_t stack[stackSize];
    uint32_t stackPointer;
    ProcessState state;
    int32_t timer;
};


#endif //TEST_PROCESS_H
