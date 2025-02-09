/**
 * @file memory.cpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Definitions from memory.hpp
 * @version 0.1
 * @date 2025-02-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <earlyLib/memory.hpp>
#include <stdint.h>
#include <stddef.h>
#include <klib/cstdlib.hpp>

const size_t mem::heapEntry_headerSize = sizeof(mem::heapEntry);

static mem::heapEntry* heap;
static mem::heapEntry* heapEnd;

void* mem::heapEntry::allocate(size_t size)
// BUG Deal with no memory available
{
    if (_next == NULL) // Reached end of list
    {
        return nullptr; // There's no memory left
    }
    
    size_t available;

    // Check if we're reserved or too small
    if (_flags.reserved || size > (available = getSize()))
    {
        return _next->allocate(size);
    }

    // We're gonna allocate this one
    // TODO maybe here set a minimum size, or list can get too big
    void* ptr = getDataPtr();

    if ( size - available > heapEntry_headerSize + 4 )
    {
        // There's space leftover, so we split the memory
        heapEntry* old_next = _next;

        _next = reinterpret_cast<heapEntry*>( 
            reinterpret_cast<uint32_t>(ptr)
            + static_cast<uint32_t>(size) );
        _next->init(old_next);
    }

    // Set the reserved flag
    _flags.reserved = true;

    return ptr;
    
}

void mem::heapEntry::free()
{
    // Reset the flags
    _flags.reserved = false;

    // Try to defragment the memory
    // FIXME: defragment should run from the beginning, what if there was a free segment behind?
    defragment();
}

void mem::heapEntry::defragment()
{
    if ( !_next->_flags.reserved ) // Next one is free
    {
        _next = _next->_next;
        defragment(); // Goes until it finds a reserved block
    }
}

void *kalloc(size_t size)
{
    return heap->allocate(size);
}

void kfree(void* ptr)
{
    mem::heapEntry* it = heap;
    while (!it->isLast())
    {
        if (it->getDataPtr() == ptr)
        {
            it->free();
            return;
        }
        it = it->getNext();
    }
    earlyPanic("kfree: no such memory address!");
}

void *operator new(size_t size)
{
    return heap->allocate(size);
}

void *operator new[](size_t size)
{
    return heap->allocate(size);
}

void operator delete(void *ptr)
{
    kfree(ptr);
}

void operator delete[](void *ptr)
{
    kfree(ptr);
}

// FIXME Use size_t size
void operator delete(void *ptr, size_t size)
{
    kfree(ptr);
}

void operator delete[](void *ptr, size_t size)
{
    kfree(ptr);
}

void mem::heapInitialize(void* ptr, size_t maxSize)
{
    heap = reinterpret_cast<mem::heapEntry*>(ptr);
    heapEnd = reinterpret_cast<mem::heapEntry*>(reinterpret_cast<uint32_t>(ptr)
                                        + maxSize - mem::heapEntry_headerSize );

    heap->init(heapEnd);

    heapEnd->init(NULL);
}