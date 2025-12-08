#ifndef DONKOS_RINGBUFFER_H
#define DONKOS_RINGBUFFER_H

#include <cstdint>

template<class TYPE, int32_t CAPACITY>
class RingBuffer {
public:
    RingBuffer() : buffer{}, head{0}, tail{0}, size{0} {
        static_assert(CAPACITY > 0, "Capacity must be > 0");
    }

    bool Push(const TYPE &data) {
        if (size < CAPACITY) {
            buffer[tail] = data;
            tail = (tail + 1) % CAPACITY;
            size++;
            return true;
        }

        return false;
    }

    bool DataAvailable() const {
        return size > 0;
    }

    bool Empty() const {
        return !DataAvailable();
    }

    void SkipReadLength() {
        Skip(ReadLength());
    }

    bool Skip(int32_t length) {
        if (length <= size) {
            head = (head + length) % CAPACITY;
            size -= length;
            return true;
        }
        return false;
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

    const TYPE *read_ptr() {
        return &buffer[head];
    }

    const TYPE *write_ptr() {
        return &buffer[tail];
    }

    int32_t WriteCapacity() const {
        return CAPACITY - size;
    }

    int32_t ReadLength() const {
        return size;
    }

private:
    TYPE buffer[CAPACITY];
    int32_t head;
    int32_t tail;
    int32_t size;
};

#endif //DONKOS_RINGBUFFER_H
