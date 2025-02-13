/**
 * @file asmIO.hpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Functions with inline assembly to directly interact with memory in real
 * mode
 * @version 0.1
 * @date 2025-02-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once
#include <stdint.h>

namespace kernel{

static inline void sendByteAssembly(uint8_t byte, uint16_t port)
{
    __asm__ __volatile__ ("outb %[b], %[p]" : : [b]"a"(byte), [p]"Nd"(port));
}

static inline uint8_t receiveByteAssembly(uint16_t port)
{
    uint8_t byte;
    __asm__ __volatile__ ("inb %[p], %[b]" : [b]"=a"(byte) : [p]"Nd"(port));
    return byte;
}

static inline uint64_t readMSR(uint32_t msr)
{
    uint32_t eax;
    uint32_t edx;
    __asm__ __volatile__ ("rdmsr" : "=a"(eax), "=d"(edx) : "c"(msr));
    return eax | (edx << 32);
}

} // namespace kernel