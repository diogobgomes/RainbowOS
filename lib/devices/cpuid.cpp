/**
 * @file cpuid.cpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Definitions from cpuid.hpp
 * @version 0.1
 * @date 2025-02-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <devices/cpu/cpuid.hpp>
#include <stdint.h>

void kernel::cpu::cpuid(uint32_t leaf, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
{
    //uint32_t a,b,c,d;

    __asm__ volatile (
        "cpuid"
        : "=a"(*eax), "=b"(*ebx), "=c"(*ecx), "=d"(*edx)
        : "a"(leaf)
        :
    );
}