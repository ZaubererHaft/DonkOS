#ifndef TEST_BINARYHEAP_H
#define TEST_BINARYHEAP_H

#include <cstdint>
#include <algorithm>

template<class T, uint32_t L>
class BinaryHeap {
public:
    BinaryHeap() : used{0U}, data{} {
    }

    int32_t Capacity() {
        return L;
    }

    int32_t Length() {
        return used;
    }

    void Insert(const T &value) {
        data[used] = value;
        if (used > 0) {
            sift(used);
        }
        used++;
    }

    T PopMax() {
        auto ret = data[0];
        used--;
        if (used > 0) {
            data[0] = data[used];
            Heapify(0);
        }
        return ret;
    }

    void Heapify(uint32_t i) {
        uint32_t l = 2 * i + 1;
        uint32_t r = 2 * i + 2;
        uint32_t largest = i;

        if (l < used && data[l]->operator>(data[i]))
            largest = l;
        if (r < used && data[r]->operator>(data[largest]))
            largest = r;

        if (largest != i) {
            std::swap(data[i], data[largest]);
            Heapify(largest);
        }
    }

    std::pair<T *, int32_t> GetArrayRepresentation() {
        return {&data[0], used};
    }


private:
    T data[L];
    uint32_t used;

    void sift(int32_t index) {
        int32_t parentIndex = (index - 1) / 2;

        while (index > 0 && data[parentIndex]->operator<(data[index])) {
            std::swap(data[index], data[parentIndex]);
            index = parentIndex;
            parentIndex = (index - 1) / 2;
        }
    }


};


#endif //TEST_BINARYHEAP_H
