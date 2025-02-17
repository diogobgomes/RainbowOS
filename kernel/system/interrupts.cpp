/**
 * @file interrupts.cpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Definitions for interrupts.hpp
 * @version 0.1
 * @date 2025-02-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <kernelInternal/system/interrupts.hpp>
#include <kernelInternal/gdt.h>
#include <klib/cstdlib.hpp>
#include <klib/asmIO.hpp>
#include <kernelInternal/devices/cpu/pic.hpp>
#include <klib/io.hpp>

extern "C" void *_handler_stub_table[];

bool kernel::interruptDescriptorTable::installInterrupt(uint8_t vector,
            void* handler, uint8_t dpl)
{
    uint32_t handler_addr = reinterpret_cast<uint32_t>(handler);

    // Vector is always valid, no need to check (max uint8_t = 255)

    // Fill the entry
    interruptDescriptor *entry = _IDT + vector;
    entry->address_low = handler_addr & 0xFFFF;
    entry->address_high = (handler_addr >> 16) & 0xFFFF;
    entry->segment_selector = CODE32_KSEGMENT;
    
    entry->flags.dpl=dpl;
    entry->flags.present = 1;
    entry->flags.type = 0xe; // 32-bit interrupt
    entry->flags.zero = 0; // Make sure it's zero

    return true;
    
}

void kernel::interruptDescriptorTable::loadIDT()
{
    interruptDescriptorTableRegister idtReg;
    idtReg.size = 0xFFF; // For practical purposes, we always have a full IDT
    idtReg.address = reinterpret_cast<uint32_t>(_IDT);

    disableInterrupts();
    __asm__ __volatile__("lidt %0" :: "m"(idtReg)); 
    enableInterrupts();
}

void kernel::interruptDescriptorTable::installAll()
{
    for (size_t i = 0; i < 256; i++)
    {
        this->installInterrupt(i,_handler_stub_table[i],0); // Leave all interrupts at ring 0 for now
    }
}

bool kernel::interruptDescriptorTable::init()
{
    if (initialized == true) // It's already enabled, can't enable again
        return false;

    // Disable PIC
    disablePIC();

    // Install all interrupts
    this->installAll();

    __asm__ __volatile__("xchgw %bx,%bx");
    // Enable Interrupts
    this->loadIDT();
    
}

void kernel::disablePIC()
{
    // BUG bochs doesn't like this
    using namespace kernel::cpu;
    kernel::sendByteAssembly(ICW_1, PIC_COMMAND_MASTER);
    kernel::sendByteAssembly(ICW_1, PIC_COMMAND_SLAVE);
    kernel::sendByteAssembly(ICW_2_MASTER, PIC_DATA_MASTER);
    kernel::sendByteAssembly(ICW_2_SLAVE, PIC_DATA_SLAVE);
    kernel::sendByteAssembly(ICW_3_MASTER, PIC_DATA_MASTER);
    kernel::sendByteAssembly(ICW_3_SLAVE, PIC_DATA_SLAVE);
    kernel::sendByteAssembly(ICW_4, PIC_DATA_MASTER);
    kernel::sendByteAssembly(ICW_4, PIC_DATA_SLAVE);
    kernel::sendByteAssembly(PIC_MASK_INTERRUPTS, PIC_DATA_MASTER);
    kernel::sendByteAssembly(PIC_MASK_INTERRUPTS, PIC_DATA_SLAVE);
}

void interruptHandler( kernel::isr_frame_t isr_frame )
{
    // BUG don't forget that we should send the EOI signal to the APIC

    // For now, let's just call an early panic
    uint32_t vector = isr_frame.intNumber;

    if (vector == 0xff) // Spurious interrupt
    {
        out << "Warning: Spurious interrupt caught\n";
        return;
    }
    

    // Build str
    char msg[] = "Interrupt called with vector xxx";

    char tempNum[MAX_NUM_STR_SIZE];
    xtoa(vector,tempNum,16);
    msg[29] = tempNum[0];
    msg[30] = tempNum[1];
    msg[31] = tempNum[2];

    earlyPanic(msg);
}
