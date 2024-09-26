#ifndef BINARYHEAP_H
#define BINARYHEAP_H

#include <algorithm>
#include <cstdint>

template<class T, int32_t L>
class BinaryHeap {
public:
    BinaryHeap() : data{}, size{0} {
    }

    bool PopMax(T &arg_Return) {
        bool tmp_Status = false;

        if (size > 0) {
            arg_Return = data[0];
            size--;

            if (size > 0) {
                data[0] = data[size];
                heapifyDown(0);
            }

            tmp_Status = true;
        }

        return tmp_Status;
    }

    bool Insert(const T &arg_Data) {
        bool tmp_Ret = false;

        if (size < L) {
            data[size] = arg_Data;
            size++;
            heapifyUp(size - 1);
            tmp_Ret = true;
        }

        return tmp_Ret;
    }

    /**
     *  \brief Tries to delete the provided data from the heap.
     *
     *  \details The alogrithm searches for the element in the underlying array using a linear search, which is of O(L). As the following heapify
     * operations has a complexity of O(log L), the overall runtime needed to delete the item is O(L). If the element occurs multiple times, the first
     * result is deleted. If the element does not exist at all, the method returns false.
     *
     * Remark: \tparam T Must overwrite == operator.
     */
    bool Delete(const T &arg_Data) {
        bool tmp_Ret = false;

        // find index of element to be deleted
        auto tmp_Index = IndexOf(arg_Data);

        if (tmp_Index >= 0) {
            // swap with last element, clear up and reduce size
            std::swap(data[tmp_Index], data[size - 1]);
            size--;
            data[size] = {};

            if (size > 0) {
                // restore invariant, if necessary
                int32_t tmp_Parent = parentIndex(tmp_Index);
                if (objectExistsAt(tmp_Parent) && data[tmp_Index] > data[tmp_Parent]) {
                    heapifyUp(tmp_Index);
                } else {
                    heapifyDown(tmp_Index);
                }
            }

            tmp_Ret = true;
        }

        return tmp_Ret;
    }

    bool Exists(const T &arg_Data) const {
        return IndexOf(arg_Data) >= 0;
    }

    int32_t IndexOf(const T &arg_Data) const {
        int32_t tmp_Index = -1;
        for (int32_t tmp_I = 0; tmp_I < size; ++tmp_I) {
            if (data[tmp_I] == arg_Data) {
                tmp_Index = tmp_I;
                break;
            }
        }
        return tmp_Index;
    }

    int32_t Size() const {
        return size;
    }

    static constexpr int32_t Capacity() {
        return L;
    }

    std::pair<const T *, int32_t> DecayToArray() const {
        return {data, size};
    }

    bool IsInvariant() const {
        bool tmp_Ret = true;
        if (size > 0) {
            tmp_Ret = isInvariantFromIndex(0);
        }
        return tmp_Ret;
    }

private:
    T data[L];
    int32_t size;

    void heapifyUp(int32_t arg_Index) {
        if (arg_Index > 0) {
            int32_t tmp_Parent = parentIndex(arg_Index);

            if (data[arg_Index] > data[tmp_Parent]) {
                std::swap(data[arg_Index], data[tmp_Parent]);
                heapifyUp(tmp_Parent);
            }
        }
    }

    void heapifyDown(int32_t arg_Index) {
        auto tmp_MaxIndex = arg_Index;
        auto tmp_LeftChildIndex = leftChildIndex(arg_Index);
        auto tmp_RightChildIndex = rightChildIndex(arg_Index);

        if (objectExistsAt(tmp_LeftChildIndex) && data[tmp_LeftChildIndex] > data[tmp_MaxIndex]) {
            tmp_MaxIndex = tmp_LeftChildIndex;
        }

        if (objectExistsAt(tmp_RightChildIndex) && data[tmp_RightChildIndex] > data[tmp_MaxIndex]) {
            tmp_MaxIndex = tmp_RightChildIndex;
        }

        // recursive sub-call to restore heap invariant
        if (tmp_MaxIndex != arg_Index) {
            std::swap(data[arg_Index], data[tmp_MaxIndex]);
            heapifyDown(tmp_MaxIndex);
        }
    }

    bool isInvariantFromIndex(int32_t arg_Index) const {
        bool tmp_Ret = true;

        auto tmp_LeftChildIndex = leftChildIndex(arg_Index);
        auto tmp_RightChildIndex = rightChildIndex(arg_Index);

        if (objectExistsAt(tmp_LeftChildIndex)) {
            if (data[tmp_LeftChildIndex] > data[arg_Index]) {
                tmp_Ret = false;
            } else {
                tmp_Ret = isInvariantFromIndex(tmp_LeftChildIndex);
            }
        }

        if (tmp_Ret && objectExistsAt(tmp_RightChildIndex)) {
            if (data[tmp_RightChildIndex] > data[arg_Index]) {
                tmp_Ret = false;
            } else {
                tmp_Ret = isInvariantFromIndex(tmp_RightChildIndex);
            }
        }

        return tmp_Ret;
    }

    static int32_t parentIndex(int32_t arg_Index) {
        return (arg_Index - 1) / 2;
    }

    static int32_t leftChildIndex(int32_t arg_ParentIndex) {
        return 2 * arg_ParentIndex + 1;
    }

    static int32_t rightChildIndex(int32_t arg_ParentIndex) {
        return 2 * arg_ParentIndex + 2;
    }

    bool objectExistsAt(int32_t arg_Index) const {
        return arg_Index < size;
    }

    bool isLeaf(int32_t arg_Index) const {
        return !(objectExistsAt(leftChildIndex(arg_Index)) || objectExistsAt(rightChildIndex(arg_Index)));
    }
};

#endif // OUTLDSHEAP_H
