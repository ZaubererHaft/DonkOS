#ifndef PROCESS_H
#define PROCESS_H

#include "MemoryListAllocator.h"

enum class ProcessState {
    CREATED, READY, WAITING, RUNNING, ENDED
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

    bool StackPtrValid();

    MemoryListAllocator &GetHeapAllocator();

    uint8_t GetPriority() const;

    void SetPriority(uint8_t prio);

    bool operator<(Process * process) const;

protected:
    void wait(int32_t milliseconds);

    uint32_t GetStackStartAddress();

    uint32_t GetStackSizeInBytes();

    uint32_t GetDataStartAddress();

    uint32_t GetDataSizeInBytes();

    void throw_ex(const char *reason);

private:
    static constexpr uint32_t stackSizeInMultipleOf4 = 256;
    static constexpr uint32_t heapSizeInMultipleOf4 = 32;

    uint32_t pid;
    std::uintptr_t heap[heapSizeInMultipleOf4];
    uint32_t stack[stackSizeInMultipleOf4];
    uint32_t stackPointer;
    ProcessState state;
    int32_t timer;
    MemoryListAllocator heapAllocator;
    uint8_t priority;

    void setReg(uint32_t &indexer, uint32_t reg);
};


#endif //TEST_PROCESS_H
