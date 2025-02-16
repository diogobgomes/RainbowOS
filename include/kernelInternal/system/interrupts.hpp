/**
 * @file interrupts.hpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Functions to set up and manage interrupts, IDT, etc
 * @version 0.1
 * @date 2025-02-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <kernelInternal/devices/cpu/cpu.hpp>

#define IDT_SIZE 256

namespace kernel
{

/**
 * @brief Flag field of the interrupt descriptor
 * 
 */
struct interruptDescriptorFlags
{
    /* Type of gate: 0x5 is Task Gate (offset should be 0) 
                     0x6 is 16-bit Interrupt Gate
                     0x7 is 16-bit Trap Gate
                     0xE is 32-bit Interrupt Gate
                     0xF is 32-bit Trap Gate */
    uint8_t         type: 4;

    /* Like the name says, should be zero */
    uint8_t         zero: 1;

    /* Defines the CPU Privilege Levels that can access this interrupt via int */
    uint8_t         dpl: 2;

    /* Present bit, must be set to 1 for valid descriptor */
    uint8_t         present: 1;
}__attribute__((packed));

static_assert(sizeof(interruptDescriptorFlags) == 1);

/**
 * @brief Interrupt descriptor
 * 
 */
struct interruptDescriptor
{
    /* Lower half of the address of the entry point of the ISR (Interrupt Service Routine) */
    uint16_t                    address_low;

    /* Segment selector, must point to valid code segment in GDT */
    uint16_t                    segment_selector;

    uint8_t                     reserved;

    /* Flags */
    interruptDescriptorFlags    flags;

    /* Higher half of the address of the entry point of the ISR */
    uint16_t address_high;
}__attribute__((packed));

static_assert(sizeof(interruptDescriptor) == 8);

struct interruptDescriptorTableRegister
{
    /* Size is one less than the IDT in bytes */
    uint16_t            size;

    /* Linear address of the IDT (paging applies) */
    uint32_t            address;
}__attribute__((packed));

static_assert(sizeof(interruptDescriptorTableRegister) == 6);

struct isr_frame_t
{
    cpu::pushad_frame   pFrame;

    uint32_t intNumber, errorCode, eip, cs, eflags;
};


/**
 * @brief Class 
 * 
 */
class interruptDescriptorTable
{
private:
    interruptDescriptor* _IDT;
    bool initialized;
    //void** _interruptHandlers;
public:
    /**
     * @brief Constructor, just reserves space for the IDT
     * 
     */
    interruptDescriptorTable()
    {
        _IDT = new interruptDescriptor[IDT_SIZE];
        initialized = false;
        //_interruptHandlers = new void*[IDT_SIZE];
    }

    /**
     * @brief Destroy the interrupt Descriptor Table object (should never run)
     * 
     */
    ~interruptDescriptorTable()
    {
        delete[] _IDT; // Should never actually run, but that's fine
        //delete[] _interruptHandlers;
    }

    /**
     * @brief Load the IDT to the IDTR with the LIDT instruction
     * 
     */
    void loadIDT();

    /**
     * @brief Install an interrupt to the IDT
     * 
     * @param vector Interrupt vector we're trying to install
     * @param handler Pointer to the Interrupt Handler
     * @param dpl What ring is allowed to call this interrupt
     * @return true If it installed the interrupt correctly
     * @return false If the install failed (possibly because it's a reserved vector)
     */
    bool installInterrupt(uint8_t vector, void* handler, uint8_t dpl);

    /**
     * @brief Fill the IDT with all interrupts
     * 
     */
    void installAll();

    /**
     * @brief Install all interrupts and enable interrupts
     * 
     */
    bool init();
};

/**
 * @brief Set the interrupt flag
 * 
 */
static inline void enableInterrupts(void) { __asm__ __volatile__ ("sti"); }

/**
 * @brief Clear the interrupt flag
 * 
 */
static inline void disableInterrupts(void) { __asm__ __volatile__ ("cli"); }

/**
 * @brief Disable the PIC
 * 
 */
void disablePIC(void);

} // namespace kernel



/**
 * @brief Main C++ interrupt handler
 * 
 * @param isr_frame Is the isr frame passed by the assembly caller
 */
extern "C" void interruptHandler( kernel::isr_frame_t isr_frame );
