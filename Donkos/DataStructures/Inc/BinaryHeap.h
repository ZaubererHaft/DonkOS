#ifndef TEST_BINARYHEAP_H
#define TEST_BINARYHEAP_H

#include <cstdint>
#include <algorithm>

template<class T, uint32_t L>
class BinaryHeap {
public:
    BinaryHeap() : used{0U}, data{} {
    }

    bool Capacity() {
        return L;
    }

    bool Length() {
        return used;
    }

    void Insert(const T &value) {
        data[used] = value;
        sift(used);
        used++;
    }

    T PopMax() {
        auto ret = data[0];
        used--;
        data[0] = data[used];
        Heapify(0);
        return ret;
    }

    void Heapify(uint32_t i) {
        uint32_t l = 2 * i + 1;
        uint32_t r = 2 * i + 2;
        uint32_t largest = i;

        if (l < used && data[l] > data[i])
            largest = l;
        if (r < used && data[r] > data[largest])
            largest = r;

        if (largest != i) {
            std::swap(data[i], data[largest]);
            Heapify(largest);
        }
    }


private:
    T data[L];
    uint32_t used;

    void sift(int32_t index) {
        int32_t parentIndex = (index - 1) / 2;

        while (index > 0 && data[parentIndex] < data[index]) {
            std::swap(data[index], data[parentIndex]);
            index = parentIndex;
            parentIndex = (index - 1) / 2;
        }
    }


};


#endif //TEST_BINARYHEAP_H
