#ifndef DONKOS_RINGBUFFER_H
#define DONKOS_RINGBUFFER_H

#include <cstdint>
#include <cstring>

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

    bool PopIntoBufferUntilMatch(TYPE end_of_pop, TYPE *target_buffer, int32_t target_buffer_size) {
        TYPE tmp;
        bool success = Pop(&tmp);
        int32_t index{};
        while (tmp != end_of_pop && success && index <= target_buffer_size) {
            target_buffer[index] = tmp;
            index++;
            success = Pop(&tmp);
        }

        if (!success || tmp != end_of_pop) {
            return false;
        }

        return true;
    }


    bool CopyFromHead(TYPE *target_buffer, int32_t target_buffer_length, int32_t length) const {
        return Copy(target_buffer, target_buffer_length, head, length);
    }


    bool Copy(TYPE *target_buffer, int32_t target_buffer_length, int32_t start, int32_t size_to_copy) const {
        //valid params?
        if (target_buffer == nullptr || size_to_copy <= 0 || start < 0 || start >= CAPACITY) {
            return false;
        }

        // all buffers are sufficient?
        if (size_to_copy > target_buffer_length || size_to_copy > size) {
            return false;
        }

        // Range check - idea: rebuild continuous interval and then check again
        // distance from head to index within the ring
        auto offset = (start - head + CAPACITY) % CAPACITY;

        // index must lie inside the valid data window
        if (offset >= size) {
            return false;
        }

        // full range must fit inside the valid data window
        if (offset + size_to_copy > size) {
            return false;
        }

        // now: perform copy
        auto head_tmp = start;
        auto bytes_until_end = CAPACITY - head_tmp;
        if (size_to_copy > bytes_until_end) {
            std::memcpy(target_buffer, &buffer[head_tmp], bytes_until_end);
            size_to_copy -= bytes_until_end;
            head_tmp = 0;
            target_buffer += bytes_until_end;
        }

        std::memcpy(target_buffer, &buffer[head_tmp], size_to_copy);
        return true;
    }

    int32_t WriteCapacity() const {
        return CAPACITY - size;
    }

    int32_t ReadLength() const {
        return size;
    }

    int32_t Head() const {
        return head;
    }

    int32_t Tail() const {
        return tail;
    }

private:
    TYPE buffer[CAPACITY];
    int32_t head;
    int32_t tail;
    int32_t size;
};

#endif //DONKOS_RINGBUFFER_H
