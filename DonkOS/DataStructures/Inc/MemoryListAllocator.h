/**
 * \file
 * \{
 * \defgroup Grp_OUtlMemListAllocator OUtlMemListAllocator
 * \{
 * \ingroup  Grp_OUtlMem
 * \brief    Class to dynamically allocate memory for a provided data buffer.
 * \details
 *
 * \author   l.kratzl
 */

#ifndef OUTLMEMLISTALLOCATOR_H_
#define OUTLMEMLISTALLOCATOR_H_

#include <cstdint>

class MemoryListAllocator {
public:
    /**
     * \brief Creates a new linked-list allocator for the given \p arg_pBuffer and \p arg_Len.
     */
    MemoryListAllocator(std::uintptr_t *arg_pBuffer, std::uintptr_t arg_Len);

    /**
     * \brief Allocates at least \p arg_RequestedSizeInBytes
     */
    void *Malloc(std::uintptr_t arg_RequestedSizeInBytes);

    /**
     * \brief Frees the memory allocated by \p arg_pAddress
     */
    void Free(void *arg_pAddress);

    /**
     * \brief Returns the memory capacity of this allocator.
     */
    std::uintptr_t GetCapacity() const;

    /**
     * \brief Returns a simple memory summary where each block is represented by two indices in the form used - length
     * \details Only for debug purposes! Note that there is NO range check, so make sure to hand over a buffer which is large enough.
     */
    void GetMemorySummary(std::uintptr_t *arg_pBuffer) const;

    /**
     * @return Returns true if this memory seems to be consistent.
     * \details This will iterate through all blocks and check if the sum of their lengths matches the buffer length and if their next´s previous fits.
     * Only for debug purposes
     */
    bool IsConsistent() const;

    /**
     * \brief Returns an estimate of the free size of this heap.
     */
    std::uintptr_t GetFree() const;

private:
    std::uintptr_t *pData;
    std::uintptr_t sizeInBytes;
    std::uintptr_t maxBlocks;
    std::uintptr_t free;
};

#endif // OUTLMEMLISTALLOCATOR_H_

/**
 * \}
 * \}
 */