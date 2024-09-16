#include "MemoryListAllocator.h"
#include "MemoryBlock.h"
#include "MemoryListAllocatorConstants.h"
#include "main.h"

#include <cstring>

MemoryListAllocator::MemoryListAllocator(std::uintptr_t *arg_pBuffer, std::uintptr_t arg_Len)
        : pData{arg_pBuffer}, sizeInBytes{arg_Len * OUtlMemConstants::st_cPtrSize},
          maxBlocks{arg_Len / (1U + OUtlMemConstants::st_cOverheadInPtrSize)},
          free{sizeInBytes} {
    if (arg_Len < OUtlMemConstants::st_cOverheadInPtrSize) {
        Error_Handler();
    }
    (void) std::memset(arg_pBuffer, 0, arg_Len * OUtlMemConstants::st_cPtrSize);
    pData[1] = sizeInBytes - OUtlMemConstants::st_cOverheadInBytes;
}

void *MemoryListAllocator::Malloc(std::uintptr_t arg_RequestedSizeInBytes) {
    void *tmp_pRet = nullptr;

    if (arg_RequestedSizeInBytes > 0U) {
        // align to size of an address
        std::uintptr_t tmp_RequestedSizeRounded =
                ((arg_RequestedSizeInBytes + OUtlMemConstants::st_cPtrSize - 1U) / OUtlMemConstants::st_cPtrSize) *
                OUtlMemConstants::st_cPtrSize;
        if ((tmp_RequestedSizeRounded & OUtlMemConstants::st_cUsedMask) <= 0U) {
            // safety: in case of potential overflows where the memory buffer might be corrupted and the loop might
            // not stop, still abort the linear search after (many) iterations with an additional condition
            std::uintptr_t tmp_Iters = maxBlocks;

            MemoryBlock tmp_CurrBlock{pData, sizeInBytes, &pData[0]};

            // first-fit search of a memory block
            while (tmp_CurrBlock.HasNext() && !tmp_CurrBlock.IsUnusedAndFits(tmp_RequestedSizeRounded) &&
                   tmp_Iters > 0U) {
                tmp_CurrBlock = tmp_CurrBlock.Next();
                tmp_Iters--;
            }

            if (tmp_CurrBlock.IsUnusedAndFits(tmp_RequestedSizeRounded) && tmp_Iters > 0U) {
                tmp_CurrBlock.Allocate(tmp_RequestedSizeRounded);
                tmp_pRet = tmp_CurrBlock.PayloadAddress();
                free -= (tmp_CurrBlock.Length() + OUtlMemConstants::st_cOverheadInBytes);
            }
        }
    }

    return tmp_pRet;
}

void MemoryListAllocator::Free(void *arg_pAddress) {
    if (arg_pAddress == nullptr) {
        Error_Handler(); // can not free a nullptr
    }

    // address exceeds known range
    if (arg_pAddress < pData || arg_pAddress >= pData + sizeInBytes / OUtlMemConstants::st_cPtrSize) {
        Error_Handler(); // unknown address
    }

    // address exceeds range after subtracting overhead
    std::uintptr_t *tmp_pCasted = reinterpret_cast<std::uintptr_t *>(arg_pAddress);
    if (tmp_pCasted - OUtlMemConstants::st_cOverheadInPtrSize < pData) {
        Error_Handler(); // illegal address
    }

    // unaligned address certainly wrong
    if (reinterpret_cast<uintptr_t>(tmp_pCasted) % OUtlMemConstants::st_cPtrSize != 0U) {
        Error_Handler(); // illegal address
    }

    MemoryBlock tmp_CurrBlock{pData, sizeInBytes, tmp_pCasted - OUtlMemConstants::st_cOverheadInPtrSize};
    if (!tmp_CurrBlock.Used()) {
        Error_Handler(); // illegal address
    }

    free += (tmp_CurrBlock.Length() + OUtlMemConstants::st_cOverheadInBytes);
    tmp_CurrBlock.Deallocate();
}

std::uintptr_t MemoryListAllocator::GetCapacity() const {
    return sizeInBytes;
}

void MemoryListAllocator::GetMemorySummary(std::uintptr_t *arg_pBuffer) const {
    uint32_t tmp_Index = 0U;
    MemoryBlock tmp_CurrBlock{pData, sizeInBytes, &pData[0]};

    arg_pBuffer[tmp_Index] = static_cast<std::uintptr_t>(tmp_CurrBlock.Used());
    arg_pBuffer[tmp_Index + 1U] = tmp_CurrBlock.Length();

    while (tmp_CurrBlock.HasNext()) {
        tmp_Index += 2U;
        tmp_CurrBlock = tmp_CurrBlock.Next();
        arg_pBuffer[tmp_Index] = static_cast<std::uintptr_t>(tmp_CurrBlock.Used());
        arg_pBuffer[tmp_Index + 1U] = tmp_CurrBlock.Length();
    }
}

bool MemoryListAllocator::IsConsistent() const {
    std::uintptr_t tmp_CurrSize = 0U;
    MemoryBlock tmp_CurrBlock{pData, sizeInBytes, &pData[0]};
    tmp_CurrSize += tmp_CurrBlock.Length() + OUtlMemConstants::st_cOverheadInBytes;

    while (tmp_CurrBlock.HasNext()) {
        if (tmp_CurrBlock.Next().Previous().GetAddress() != tmp_CurrBlock.GetAddress()) {
            tmp_CurrSize = 0U;
            break;
        }
        tmp_CurrBlock = tmp_CurrBlock.Next();
        tmp_CurrSize += tmp_CurrBlock.Length() + OUtlMemConstants::st_cOverheadInBytes;
    }

    return tmp_CurrSize == sizeInBytes;
}

std::uintptr_t MemoryListAllocator::GetFree() const {
    return free;
}
