/**
 * @file memoryDetection16.cpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Defintions from earlyLib/memoryDetection16.hpp
 * @version 0.1
 * @date 2025-02-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <earlyLib/memoryDetection16.hpp>
#include <earlyLib/realMode.hpp>
#include <klib/cstdlib.hpp>
#include <stdint.h>

#ifdef TRACEMAX
    #include<klib/tracemax.hpp>
#endif


uint32_t mem::queryLowerMemory()
{
    #ifdef TRACEMAX
        traceOut << "Entering queryLowerMemory\n";
    #endif


    uint32_t result = 0;
    realModeCall(&asmCall_int12,reinterpret_cast<uint32_t>(&result));

    if (result == 0) // We had a failure to detect the lower memory
    {
        earlyPanic("queryLowerMemory(): Failure to query lower memory!");
    }
    
    return result;
}

const static uint32_t eaxSignature = 0x534D4150;

mem::upperMemory_mmap_struct* mem::queryUpperMemory()
{
    #ifdef TRACEMAX
        traceOut << "Entering queryUpperMemory()\n";
    #endif

    size_t size=0;
    const size_t maxSize = 100; // Let's try this number
    auto mmapPtr = new mmap_structure_entry[maxSize];

    uint32_t ebx = 0;
    uint32_t eax = 0;
    uint32_t ecx = 0;
    uint32_t carry = 0;

    do
    {
        // Current entry
        auto currPtr = mmapPtr + size;

        // Calculate %es:%di
        uint32_t offset = reinterpret_cast<uint32_t>(currPtr) % 16;
        uint32_t segment = (reinterpret_cast<uint32_t>(currPtr) - offset) / 16;

        // Actually call the function
        realModeCall(&asmCall_int15,offset,segment,ebx,&ebx,&eax,&ecx,&carry);

        // Check for carry
        if ( carry )
            earlyPanic("queryUpperMemory(): Failure! Carry was set");

        // Check for signature
        if ( eax != eaxSignature )
            earlyPanic("queryUpperMemory(): Failure! %eax is different than signature");

        // Test entry to see if we discard it
        if ( ecx <= 20 || (currPtr->acpi_3_0_extended & 0x0001) == 0 ) // Keep entry
        {
            size++;
        }
        else
        {
                        #ifdef TRACEMAX
                traceOut << "Not keeping this entry\n";
            #endif
        }

        // Test to see if we're getting too big
        if (size >= maxSize)
            earlyPanic("queryUpperMemory(): Failure! List got a little too big");
    } while (ebx != 0);
    
    auto returnStruct = new upperMemory_mmap_struct;
    returnStruct->size = size;
    returnStruct->ptr = mmapPtr;
    return returnStruct;
}

uint32_t mem::getUpperMemorySize( const upperMemory_mmap_struct* ptr)
{
    size_t numEntries = ptr->size;
    mmap_structure_entry* mmapPtr = ptr->ptr;

    uint32_t finalSize = 0;

    // BUG This test should become unnecessary
    for ( const struct mmap_structure_entry* entry = mmapPtr; entry < mmapPtr + numEntries; ++entry)
    {
        uint64_t test = finalSize + entry->length;
        if (test > UINT32_MAX) return 0;
        finalSize = uint32_t(test); // It's safe cause we tested it!
    }
    
    return finalSize;
}