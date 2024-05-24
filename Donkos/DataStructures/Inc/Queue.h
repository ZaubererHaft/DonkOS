#ifndef TEST_QUEUE_H
#define TEST_QUEUE_H

#include <cstdint>
#include <utility>

template<class T, uint32_t L>
class Queue {
public:
    Queue() : start{0U}, end{0U} {

    }

    T Push(const T &value) {
        data[end] = value;
        end = (end + 1) % L;
    }

    T Pop() {
        T ret = data[start];
        start = (start + 1) % L;
        return ret;
    }

    uint32_t Length() const {
        return end - start;
    };

    uint32_t Capacity() const {
        return L;
    }

private:
    T data[L];
    uint32_t start;
    uint32_t end;
};

#endif //TEST_QUEUE_H
