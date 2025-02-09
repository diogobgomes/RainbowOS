/**
 * @file memoryDetection16.hpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Functions to detect memory in 16-bit real mode using int 12h and 15h
 * @version 0.1
 * @date 2025-02-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <sys/multiboot.h>

namespace mem
{

struct upperMemory_mmap_struct
{
    size_t size;
    mmap_structure_entry* ptr;
};


uint32_t queryLowerMemory();

upperMemory_mmap_struct* queryUpperMemory();

uint32_t getUpperMemorySize( const upperMemory_mmap_struct* ptr );


} // namespace mem

extern "C" {
    uint32_t asmCall_int12(uint32_t returnPtr);
    uint32_t asmCall_int15(uint32_t offset, uint32_t segment, uint32_t ebx, uint32_t ebxPtr, uint32_t eaxPtr, uint32_t ecxPtr, uint32_t carryPtr);
}