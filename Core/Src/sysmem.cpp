/**
 ******************************************************************************
 * @file      sysmem.c
 * @author    Generated by STM32CubeIDE
 * @brief     STM32CubeIDE System Memory calls file
 *
 *            For more information about which C functions
 *            need which of these lowlevel functions
 *            please consult the newlib libc manual
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes */
#include <errno.h>
#include <stdint.h>
#include <malloc.h>
#include <new>
#include <cstdlib>
#include "main.h"
#include "../../Donkos/DonkosInternal.h"

/**
 * Pointer to the current high watermark of the heap usage
 */
static uint8_t *__sbrk_heap_end = NULL;

/**
 * @brief _sbrk() allocates memory to the newlib heap and is used by malloc
 *        and others from the C library
 *
 * @verbatim
 * ############################################################################
 * #  .data  #  .bss  #       newlib heap       #          MSP stack          #
 * #         #        #                         # Reserved by _Min_Stack_Size #
 * ############################################################################
 * ^-- RAM start      ^-- _end                             _estack, RAM end --^
 * @endverbatim
 *
 * This implementation starts allocating at the '_end' linker symbol
 * The '_Min_Stack_Size' linker symbol reserves a memory for the MSP stack
 * The implementation considers '_estack' linker symbol to be RAM end
 * NOTE: If the MSP stack, at any point during execution, grows larger than the
 * reserved size, please increase the '_Min_Stack_Size'.
 *
 * @param incr Memory size
 * @return Pointer to allocated memory
 */
extern "C" void *_sbrk(ptrdiff_t incr) {

    extern uint8_t _end; /* Symbol defined in the linker script */
    extern uint8_t _estack; /* Symbol defined in the linker script */
    extern uint32_t _Min_Stack_Size; /* Symbol defined in the linker script */
    const uint32_t stack_limit = (uint32_t) &_estack - (uint32_t) &_Min_Stack_Size;
    const uint8_t *max_heap = (uint8_t *) stack_limit;
    uint8_t *prev_heap_end;

    /* Initialize heap end at first call */
    if (NULL == __sbrk_heap_end) {
        __sbrk_heap_end = &_end;
    }

    /* Protect heap from growing into the reserved MSP stack */
    if (__sbrk_heap_end + incr > max_heap) {
        errno = ENOMEM;
        return (void *) -1;
    }

    prev_heap_end = __sbrk_heap_end;
    __sbrk_heap_end += incr;
    return (void *) prev_heap_end;

}

void *MyNew(std::size_t arg_Size) {
    bool useMSP = (__get_CONTROL() & 0b10) == 0;
    void *tmp_pPtr;

    if (useMSP) {
        tmp_pPtr = std::malloc(arg_Size);
    } else {
        tmp_pPtr = Donkos_GetScheduler().GetCurrentProcess()->GetHeapAllocator().Malloc(arg_Size);
    }
    return tmp_pPtr;
}

void MyDelete(void *arg_pPtr) {
    bool useMSP = (__get_CONTROL() & 0b10) == 0;
    if (useMSP) {
        std::free(arg_pPtr);
    } else {
        Donkos_GetScheduler().GetCurrentProcess()->GetHeapAllocator().Free(arg_pPtr);
    }
}

void *operator new(std::size_t arg_Size) {
    return MyNew(arg_Size);
}

void *operator new[](std::size_t arg_Size) {
    return MyNew(arg_Size);
}

void operator delete(void *arg_pPtr) noexcept {
    MyDelete(arg_pPtr);
}

void operator delete(void *arg_pPtr, std::size_t arg_Size) noexcept {
    MyDelete(arg_pPtr);
}

void operator delete[](void *arg_pPtr) noexcept {
    MyDelete(arg_pPtr);
}

void operator delete[](void *arg_pPtr, std::size_t arg_Size) noexcept {
    MyDelete(arg_pPtr);
}