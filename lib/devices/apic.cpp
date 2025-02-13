/**
 * @file apic.cpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <devices/cpu/apic.hpp>
#include <stdint.h>
#include <devices/cpu/cpuid.hpp>
#include <devices/cpu/msr.hpp>

bool kernel::cpu::checkApic()
{
    uint32_t a,b,c,d;
    cpuid(1,&a,&b,&c,&d);
    return d & static_cast<uint32_t>(cpuid_features::CPUID_FEAT_EDX_APIC);
}

kernel::cpu::lapic_base_register* kernel::cpu::getAPICRegister()
{
    uint32_t a,b,c,d;
    cpuid(1,&a,&b,&c,&d);
    return reinterpret_cast<lapic_base_register*>(a);
}

void kernel::cpu::enableAPIC()
{
    /* Get the APIC register */
    lapic_base_register* lapicRegAddr = getAPICRegister();

    /* Set enabled bit */
    setMSR(IA32_APIC_BASE_MSR,reinterpret_cast<uint32_t>(lapicRegAddr) & 0xfffff000,0);

    /* Set the Spurious Interrupt Vector register enable bit to start receiving interrupts */

    uint32_t* lapicReg = reinterpret_cast<uint32_t*>(lapicRegAddr->address << 12);

    auto regPtr = reinterpret_cast<spurious_interrupt_vector_register*>
        (lapicReg + static_cast<uint32_t>(lapic_registers::SPURIOUS_INTERRUPT_VECTOR));

    regPtr->enable_flag = 1;

    regPtr->sp_vector = 0xff; // Set spurious interrupt to 0xff
}

