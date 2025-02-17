/**
 * @file init/main.cpp
 * @author Diogo Gomes
 * @brief 32-bit protected mode kernel
 * @version 0.1
 * @date 2025-02-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <sys/multiboot.h>
#include <klib/io.hpp>
#include <klib/cstdlib.hpp>
#include <devices/BIOSVideoIO.hpp>
#include <earlyLib/memory.hpp>
#include <kernelInternal/system/interrupts.hpp>
#include <kernelInternal/devices/cpu/cpuid.hpp>
#include <kernelInternal/devices/cpu/apic.hpp>
#include <kernelInternal/acpiKernel.hpp>
#include <debug.h>


extern "C" {
    [[noreturn]] void kmain( uint32_t multiboot_flag,
            const struct multiboot_info_structure* info, uint32_t terminalIndex );
}

extern uintptr_t _endSymbol;

// Variables
io::_outstream<io::framebuffer_terminal> out;

void kmain( uint32_t multiboot_flag,
            const struct multiboot_info_structure* info, uint32_t terminalIndex )
{
    // Initialize the terminal
    io::framebuffer_terminal initTerminal;
    out.init(&initTerminal);
    out.hex();

    // Test to see if we were booted from custom bootloader
    if (multiboot_flag == MULTIBOOT_CUSTOM_BOOTLOADER_MAGIC)
    {
        uint8_t _row = uint8_t(terminalIndex / initTerminal.vga_width);
        uint8_t _column = uint8_t( terminalIndex - _row * initTerminal.vga_width );
        initTerminal.setCursor(_row,_column);
    }
    else // Not loaded by custom bootloader, clear the screen
    {
        initTerminal.clear();
    }

    // Print header
    initTerminal.setColor(io::vga_color::VGA_COLOR_LIGHT_GREY,
                            io::vga_color::VGA_COLOR_BLUE);
    out << "Entering RainbowOS 32-bit protected mode kernel.bin\n";
    initTerminal.setColor(io::vga_color::VGA_COLOR_LIGHT_GREY,
                            io::vga_color::VGA_COLOR_BLACK);

    // Set up temporary heap, 4K should be enough
    void* endOfBinary = &_endSymbol; // This is a linker symbol, end of binary in memory
    out << "Creating a temporary heap at 0x" << endOfBinary << " of size 4k\n";
    mem::heapInitialize(endOfBinary,4*1024);

    // Check CPUID
    if (check_CPUID_available())
    {
        out << "CPUID is supported\n";
    } else earlyPanic("Error: CPUID is not supported, aborting!");

    // Finding ACPI
    kernel::acpi::acpi_header acpiHeader;
    if(acpiHeader.getType() == 1)
        out << "We have a type 1 ACPI table\n";
    else
        out << "We have a type 2 ACPI table\n";

    // Finding MADT
    kernel::acpi::acpi_madt madt(&acpiHeader);
    auto type1 = reinterpret_cast<kernel::acpi::madt_entry_type1*>(madt.getEntry(1));

    out << "Found type 1 MADT @ " << reinterpret_cast<uint32_t>(type1) << "\n";

    kernel::cpu::io_apic ioAPIC(&madt);

    out << "IO APIC is good\n";

    // LAPIC
    kernel::cpu::l_apic localAPIC;
    localAPIC.enable();

    out << "Enabled local APIC\n";

    out << "Attempting to enable interrupts\n";
    
    kernel::interruptDescriptorTable idt;

    idt.init();

    out << "Are they enabled?...\n";

    BOCHS_STOP
    __asm__ __volatile__ ("int $0x34");
    
    /*
    // Try to find ACPI headers
    auto RSDPDescPtr = kernel::acpi::findRSDPDesc();
    if (RSDPDescPtr == nullptr)
        earlyPanic("Error: Could not find the XSDP, aborting!");
    else if(RSDPDescPtr->revision==0)
        out << "Why god oh why???\n";
    else if(RSDPDescPtr->revision!=2) // Only support xsdp
        earlyPanic("Error: ACPI is not XSDP, aborting!");
    else
        out << "Found ACPI tables at location " <<
            reinterpret_cast<uint32_t>(RSDPDescPtr) << "\n";

    // Setup Interrupts
    // LAPIC
    out << "Enabling APIC\n";
    kernel::cpu::enableAPIC();

    // IOAPIC
    kernel::cpu::io_apic ioAPIC(RSDPDescPtr);

    out << "IOAPIC found at address " << reinterpret_cast<uint32_t>(ioAPIC._ptr) << "\n" ;

    out << "Attempting to enable interrupts\n";
    
    kernel::interruptDescriptorTable idt;

    idt.init();

    out << "Are they enabled?...\n";*/

    earlyPanic("Shouldn't be seeing this!!!");
}
