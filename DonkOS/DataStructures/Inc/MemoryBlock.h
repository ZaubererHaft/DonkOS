/**
 * \file
 * \{
 * \defgroup  Grp_OUtlMemBlock OUtlMemBlock
 * \{
 * \ingroup   Grp_OUtlMem
 * \brief     Class representing a block of memory.
 * \details
 *
 * \author   l.kratzl
 */

#ifndef OUTLMEMBLOCK_H_
#define OUTLMEMBLOCK_H_

#include <cstdint>

class MemoryBlock {
public:
    /**
     * \details Creates a new block of memory belonging to an allocator.
     *
     * @param arg_pBuffer The global buffer where the block allocates a certain range.
     * @param arg_Len  The global length of the provided buffer.
     * @param arg_pAddress The address of memory in the \p arg_pBuffer where this block will be located at.
     */
    MemoryBlock(std::uintptr_t *arg_pBuffer, std::uintptr_t arg_Len, std::uintptr_t *arg_pAddress);

    /**
     * \brief Returns true if there is a neighboring block.
     * \details This will check if the higher memory address is a) available and b) has a length of > 0
     */
    bool HasNext() const;

    /**
     * \brief Returns the next block of memory.
     * \details This method DOES NOT check if the block exists or not. Use HasNext() for that purpose.
     */
    MemoryBlock Next() const;

    /**
     * \brief Returns true if this block has enough memory available for the requested size in bytes and is unused.
     */
    bool IsUnusedAndFits(std::uintptr_t arg_Len) const;

    /**
     * \brief Allocates the requested memory in this block and splits a next one (subtracting the requested size and overhead) if possible.
     */
    void Allocate(std::uintptr_t arg_RequestedSize);

    /**
     * \brief Returns the address where potential payload might start.
     */
    void *PayloadAddress();

    /**
     * \brief Merges this block with the next and previous ones, if available, and sets this block to unused.
     */
    void Deallocate();

    /**
     * \brief Returns the length of this block in bytes.
     */
    std::uintptr_t Length() const;

    /**
     * \brief Returns true if this block is free or not. This will use the used mask from the constants header.
     */
    bool Used() const;

    /**
     * \brief Returns the previous block. this does NOT check if the block exists.
     */
    MemoryBlock Previous();

    /**
     * \brief Returns the memory address that is maintained by this block
     */
    const std::uintptr_t *GetAddress() const;

private:
    std::uintptr_t *pBufferAddress;
    std::uintptr_t bufferLenInBytes;
    std::uintptr_t *pMyAddress;

    /**
     * \brief Marks this block as free. This will use the used mask from the constants header.
     */
    void setUnused();

    /**
     * \brief Sets the length of this block in bytes.
     */
    void setLength(std::uintptr_t arg_Len);

    /**
     * \brief Returns true if a previous block exists (first byte of the block header)
     */
    bool hasPrevious() const;

    /**
     * \brief Sets a previous block (first byte of the block header)
     */
    void setPrevious(const std::uintptr_t *arg_pAddress);

    /**
     * \brief Returns the address of the next block by calculating address + length + overhead.
     */
    std::uintptr_t *nextAddress() const;

    /**
     * \brief Tries to merge this block with a (right) neighboring.
     */
    void tryMergeWith(MemoryBlock &arg_Block);
};

#endif // OUTLMEMBLOCK_H_

/**
 * \}
 * \}
 */