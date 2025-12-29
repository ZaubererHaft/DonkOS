#ifndef TEST_QUEUE_H
#define TEST_QUEUE_H

#include <cstdint>
#include <utility>

template<class T, uint32_t L>
class Queue {
public:
    Queue() : start{0U}, end{0U} {

    }

    bool Push(const T &value) {
        if (size == L)
            return false; // full

        data[end] = value;
        end = (end + 1) % L;
        ++size;
        return true;
    }

    T Pop() {
        T out = data[start];
        start = (start + 1) % L;
        --size;
        return out;
    }

    uint32_t Length() const {
        return size;
    }

    uint32_t Capacity() const {
        return L;
    }

private:
    T data[L];
    uint32_t start;
    uint32_t end;
    uint32_t size;
};

#endif //TEST_QUEUE_H
