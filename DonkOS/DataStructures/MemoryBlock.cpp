#include "MemoryBlock.h"
#include "MemoryListAllocatorConstants.h"

MemoryBlock::MemoryBlock(std::uintptr_t *arg_pBuffer, std::uintptr_t arg_Len, std::uintptr_t *arg_pAddress)
        : pBufferAddress{arg_pBuffer}, bufferLenInBytes{arg_Len}, pMyAddress{arg_pAddress} {
}

bool MemoryBlock::HasNext() const {
    bool tmp_Ret = false;
    std::uintptr_t *tmp_pNextAddr = nextAddress();
    std::uintptr_t *tmp_pLastAddr = pBufferAddress + bufferLenInBytes / OUtlMemConstants::st_cPtrSize;

    // make sure to not exceed the available buffer range and then check if used flag is set
    if (tmp_pNextAddr <= tmp_pLastAddr - OUtlMemConstants::st_cOverheadInPtrSize) {
        tmp_Ret = (tmp_pNextAddr[0] & OUtlMemConstants::st_cLenMask) > 0U;
    }
    return tmp_Ret;
}

MemoryBlock MemoryBlock::Next() const {
    return {pBufferAddress, bufferLenInBytes, nextAddress()};
}

bool MemoryBlock::IsUnusedAndFits(std::uintptr_t arg_Len) const {
    return !Used() && Length() >= arg_Len;
}

bool MemoryBlock::Used() const {
    return (pMyAddress[1] & OUtlMemConstants::st_cUsedMask) > 0U;
}

void MemoryBlock::setUnused() {
    pMyAddress[1] &= OUtlMemConstants::st_cLenMask;
}

void MemoryBlock::Allocate(std::uintptr_t arg_RequestedSize) {
    std::uintptr_t tmp_InitialBlockLen = Length();
    std::uintptr_t tmp_RemainingLen = tmp_InitialBlockLen - arg_RequestedSize;

    if (tmp_RemainingLen > OUtlMemConstants::st_cOverheadInBytes) {
        // the length needs to be updated here s.t. nextAddress returns the correct neighboring address
        pMyAddress[1] = OUtlMemConstants::st_cUsedMask | arg_RequestedSize;

        MemoryBlock tmp_NewBlock{pBufferAddress, bufferLenInBytes, nextAddress()};
        tmp_NewBlock.setLength(tmp_RemainingLen - OUtlMemConstants::st_cOverheadInBytes);
        tmp_NewBlock.setPrevious(pMyAddress);
        tmp_NewBlock.setUnused();

        // update the next's previous as well
        if (tmp_NewBlock.HasNext()) {
            tmp_NewBlock.Next().setPrevious(tmp_NewBlock.pMyAddress);
        }
    } else {
        pMyAddress[1] = OUtlMemConstants::st_cUsedMask | (arg_RequestedSize + tmp_RemainingLen);
    }
}

void *MemoryBlock::PayloadAddress() {
    return &pMyAddress[2];
}

void MemoryBlock::Deallocate() {
    setUnused();

    if (HasNext()) {
        MemoryBlock tmp_Next = Next();
        tryMergeWith(tmp_Next);
    }

    if (hasPrevious()) {
        MemoryBlock tmp_Previous = Previous();
        tmp_Previous.tryMergeWith(*this);
    }
}

void MemoryBlock::tryMergeWith(MemoryBlock &arg_Block) {
    if (!Used() && !arg_Block.Used()) {
        uintptr_t tmp_Len = Length() + arg_Block.Length() + OUtlMemConstants::st_cOverheadInBytes;
        setLength(tmp_Len);

        // update previous address of next's next (if available)
        if (arg_Block.HasNext()) {
            arg_Block.Next().setPrevious(pMyAddress);
        }

        // clean up next address in either case (not necessary as block is anyway unused but makes for cleaner memory layout)
        arg_Block.pMyAddress[1] = 0U;
        arg_Block.setPrevious(nullptr);
    }
}

std::uintptr_t MemoryBlock::Length() const {
    return pMyAddress[1] & OUtlMemConstants::st_cLenMask;
}

void MemoryBlock::setLength(std::uintptr_t arg_Len) {
    pMyAddress[1] &= OUtlMemConstants::st_cUsedMask;
    pMyAddress[1] |= arg_Len;
}

std::uintptr_t *MemoryBlock::nextAddress() const {
    return pMyAddress + OUtlMemConstants::st_cOverheadInPtrSize + Length() / OUtlMemConstants::st_cPtrSize;
}

void MemoryBlock::setPrevious(const std::uintptr_t *arg_pAddress) {
    pMyAddress[0] = reinterpret_cast<uintptr_t>(arg_pAddress);
}

MemoryBlock MemoryBlock::Previous() {
    return {pBufferAddress, bufferLenInBytes, reinterpret_cast<std::uintptr_t *>(pMyAddress[0])};
}

const std::uintptr_t *MemoryBlock::GetAddress() const {
    return pMyAddress;
}

bool MemoryBlock::hasPrevious() const {
    return pMyAddress[0] != 0U;
}
