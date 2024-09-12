#ifndef TEST_PROCESS_H
#define TEST_PROCESS_H

#include <cstdint>

enum class ProcessState {
    CREATED, READY, WAITING, RUNNING
};

class Process {
public:
    static constexpr uint32_t stackSizeInMultipleOf4 = 1024;
    static constexpr uint32_t heapSizeInMultipleOf4 = 32;

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

    bool StackPtrValid();

    std::uintptr_t *GetHeap();

protected:
    void wait(int32_t milliseconds);

private:
    uint32_t pid;
    std::uintptr_t heap[heapSizeInMultipleOf4];
    uint32_t stack[stackSizeInMultipleOf4];
    uint32_t stackPointer;
    ProcessState state;
    int32_t timer;

    void setReg(uint32_t &indexer, uint32_t reg);
};


#endif //TEST_PROCESS_H
