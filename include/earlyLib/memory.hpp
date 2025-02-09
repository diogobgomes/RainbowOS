/**
 * @file memory.hpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Basic memory management functions for the bootloader, malloc/new, free/delete
 * @version 0.1
 * @date 2025-02-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

extern "C" {
    void *kalloc(size_t size);
    void kfree(void *ptr);
}

void *operator new(size_t size);

void *operator new[](size_t size);

void operator delete(void *ptr);

void operator delete[](void *ptr);

void operator delete(void *ptr, size_t size);

void operator delete[](void *ptr, size_t size);

namespace mem
{

/**
 * @brief Flags to be used by the heap class
 * 
 */
struct heapFlags
{
    bool reserved : 1;
};

extern const size_t heapEntry_headerSize;
    
class heapEntry
{
public:
    /**
     * @brief Init function, initializes an entry
     * 
     * @param next Pointer to the next entry (if it's the last, it'll mark it reserved)
     */
    void init(heapEntry* next) { _flags.reserved = next ? false : true; _next = next;}

    /**
     * @brief Attempt to allocate a memory of size size
     * 
     * @param size Size of memory to allocate
     * @return void* Pointer to data buffer, if NULL, then there wasn't enough memory
     */
    void *allocate(size_t size);

    /**
     * @brief Frees the block, and tries (badly) to coalesce the blocks around
     * 
     */
    void free();

    /**
     * @brief Get the size of the data buffer
     * 
     * @return size_t 
     */
    size_t getSize() { return reinterpret_cast<size_t>(_next) -
                              reinterpret_cast<size_t>(this) - heapEntry_headerSize; }

    /**
     * @brief Get the pointer to the data buffer
     * 
     * @return void* 
     */
    void *getDataPtr() { return reinterpret_cast<void*>(
                            reinterpret_cast<uint32_t>(this) + heapEntry_headerSize); }

    /**
     * @brief Checks if it's the last entry
     * 
     * @return true It's the last entry
     * @return false It's not the last entry
     */
    bool isLast() { return _next ? false : true; }

    /**
     * @brief Get the pointer to the next entry
     * 
     * @return heapEntry* 
     */
    heapEntry* getNext() { return _next; }

private:
    heapEntry* _next; // Pointer to next object
    heapFlags _flags; // Flags
    void defragment(); // Tries to defragment, it's shit

}; // class heapEntry

/**
 * @brief Initializes the heap
 * 
 * @param ptr Pointer to the heap location
 * @param maxSize Maximum size of the heap
 */
void heapInitialize(void* ptr, size_t maxSize);


} // namespace mem


