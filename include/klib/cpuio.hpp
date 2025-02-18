/**
 * @file cpuio.hpp
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

namespace kernel::cpu::io
{

static const int io_port = 0x80;

static inline void outb(uint16_t port, uint8_t byte)
{
    __asm__ __volatile__ ("outb %[b], %[p]" : : [b]"a"(byte), [p]"Nd"(port));
}

static inline uint8_t inb(uint16_t port)
{
    uint8_t byte;
    __asm__ __volatile__ ("inb %[p], %[b]" : [b]"=a"(byte) : [p]"Nd"(port));
    return byte;
}

static inline void iowait()
{
    outb(io_port,0);
}

} // namespace kernel