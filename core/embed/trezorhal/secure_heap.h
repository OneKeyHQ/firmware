// clang-format off

/*
 * FreeRTOS Kernel V10.4.6
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef __SECURE_HEAP_H__
#define __SECURE_HEAP_H__

#include <stddef.h>

#include "sdram.h"

#define configSUPPORT_DYNAMIC_ALLOCATION 1
#define configTOTAL_HEAP_SIZE ((size_t)FMC_SDRAM_USER_HEAP_LEN)
#define portMAX_DELAY 0xffffffff

#define PRIVILEGED_DATA
#define PRIVILEGED_FUNCTION

#define portBYTE_ALIGNMENT    8
#define portBYTE_ALIGNMENT_MASK    ( 0x0007 )

#define taskENTER_CRITICAL()
#define taskEXIT_CRITICAL()

#define vTaskSuspendAll()
#define mtCOVERAGE_TEST_MARKER()
#define traceMALLOC( pvReturn, xWantedSize )
#define traceFREE( pv, xBlockSize )
#define configASSERT( x )

typedef struct xHeapStats
{
    size_t xAvailableHeapSpaceInBytes;      /* The total heap size currently available - this is the sum of all the free blocks, not the largest block that can be allocated. */
    size_t xSizeOfLargestFreeBlockInBytes;  /* The maximum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. */
    size_t xSizeOfSmallestFreeBlockInBytes; /* The minimum size, in bytes, of all the free blocks within the heap at the time vPortGetHeapStats() is called. */
    size_t xNumberOfFreeBlocks;             /* The number of free memory blocks within the heap at the time vPortGetHeapStats() is called. */
    size_t xMinimumEverFreeBytesRemaining;  /* The minimum amount of total free memory (sum of all free blocks) there has been in the heap since the system booted. */
    size_t xNumberOfSuccessfulAllocations;  /* The number of calls to pvPortMalloc() that have returned a valid memory block. */
    size_t xNumberOfSuccessfulFrees;        /* The number of calls to vPortFree() that has successfully freed a block of memory. */
} HeapStats_t;

/* Standard includes. */
#include <stdint.h>

/**
 * @brief Allocates memory from heap.
 *
 * @param[in] xWantedSize The size of the memory to be allocated.
 *
 * @return Pointer to the memory region if the allocation is successful, NULL
 * otherwise.
 */
void * pvPortMalloc( size_t xWantedSize );

void * pvPortReMalloc( void * pv , size_t xWantedSize );

/**
 * @brief Frees the previously allocated memory.
 *
 * @param[in] pv Pointer to the memory to be freed.
 */
void vPortFree( void * pv );

/**
 * @brief Get the free heap size.
 *
 * @return Free heap size.
 */
size_t xPortGetFreeHeapSize( void );

/**
 * @brief Get the minimum ever free heap size.
 *
 * @return Minimum ever free heap size.
 */
size_t xPortGetMinimumEverFreeHeapSize( void );

#endif /* __SECURE_HEAP_H__ */
