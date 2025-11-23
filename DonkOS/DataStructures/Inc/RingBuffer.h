#ifndef DONKOS_RINGBUFFER_H
#define DONKOS_RINGBUFFER_H

#include <cstdint>

template<class TYPE, int32_t CAPACITY>
class RingBuffer {
public:
    RingBuffer() : buffer{}, head{0}, tail{0}, size{0} {
        static_assert(CAPACITY > 0, "Capacity must be > 0");
    }

    void Push(const TYPE &data) {
        buffer[tail] = data;
        tail = (tail + 1) % CAPACITY;

        // In case the capacity of the buffer was reached we restart at the beginning and advance head as well
        if (size < CAPACITY) {
            size++;
        } else {
            head = (head + 1) % CAPACITY;
        }
    }

    bool DataAvailable() const {
        return size > 0;
    }

    bool Empty() const {
        return !DataAvailable();
    }

    bool Pop(TYPE *result) {
        if (DataAvailable()) {
            *result = buffer[head];
            head = (head + 1) % CAPACITY;
            size--;
            return true;
        }

        return false;
    }

private:
    TYPE buffer[CAPACITY];
    int32_t head;
    int32_t tail;
    int32_t size;
};

#endif //DONKOS_RINGBUFFER_H
