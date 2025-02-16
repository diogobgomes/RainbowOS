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

#include <kernelInternal/devices/cpu/apic.hpp>
#include <stdint.h>
#include <kernelInternal/devices/cpu/cpuid.hpp>
#include <kernelInternal/devices/cpu/msr.hpp>
#include <kernelInternal/devices/cpu/apic.hpp>
#include <klib/string.h>
#include <klib/cstdlib.hpp>
//#include <klib/io.hpp>

using namespace kernel::cpu;

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

/**========================================================================
 *                           CLASS io_apic
 *========================================================================**/
io_apic::io_apic(kernel::acpi::acpi_madt* ptr)
{
    // Try to get a madt type 1 entry
    auto entry = ptr->getEntry(1);

    if(entry == nullptr)
        earlyPanic("In kernel::cpu::io_apic constructor: Error: Couldn't find a MADT type 1 entry!");
    
    _ptr = reinterpret_cast<kernel::acpi::madt_entry_type1*>(entry);
}

uint32_t io_apic::read(ioapic_mm_register reg)
{
    // Volatile for no reorder memory access
    uint32_t volatile *ioapicPtr = reinterpret_cast<uint32_t volatile *>
            (_ptr->io_apic_address);
    ioapicPtr[0] = (static_cast<uint32_t>(reg) & 0xff);
    return ioapicPtr[4];
}

void io_apic::write(ioapic_mm_register reg, uint32_t value)
{
    // Volatile for no reorder memory access
    uint32_t volatile *ioapicPtr = reinterpret_cast<uint32_t volatile *>
            (_ptr->io_apic_address);
    ioapicPtr[0] = (static_cast<uint32_t>(reg) & 0xff);
    ioapicPtr[4] = value;
}