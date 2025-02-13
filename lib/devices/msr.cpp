/**
 * @file msr.cpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Definitions from msr.hpp
 * @version 0.1
 * @date 2025-02-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <devices/cpu/msr.hpp>
#include <devices/cpu/cpuid.hpp>

bool kernel::cpu::checkMSR()
{
    static uint32_t a,b,c,d;
    cpuid(1,&a,&b,&c,&d);
    return d & static_cast<uint32_t>(cpuid_features::CPUID_FEAT_EDX_MSR);
}

void kernel::cpu::getMSR(uint32_t msr, uint32_t *low, uint32_t *high)
{
    __asm__ volatile(
        "rdmsr"
        : "=a"(*low), "=d"(*high)
        : "c"(msr)
    );
}

void kernel::cpu::setMSR(uint32_t msr, uint32_t low, uint32_t high)
{
    __asm__ volatile(
        "wrmsr"
        : : "a"(low), "d"(high), "c"(msr)
    );
}