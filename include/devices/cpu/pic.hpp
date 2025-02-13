/**
 * @file pic.hpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Definitions for interacting with the PIC
 * @version 0.1
 * @date 2025-02-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <stdint.h>

namespace kernel::cpu
{
    // Ports
    static const uint8_t PIC_COMMAND_MASTER =   0x20;
    static const uint8_t PIC_DATA_MASTER =      0x21;
    static const uint8_t PIC_COMMAND_SLAVE =    0xa0;
    static const uint8_t PIC_DATA_SLAVE =       0xa1;

    // Initialization Command Words
    static const uint8_t ICW_1 =                0x11; // Initialization sequence
    static const uint8_t ICW_2_MASTER =         0x21; // Interrupt vector address values
    static const uint8_t ICW_2_SLAVE =          0x28;
    static const uint8_t ICW_3_MASTER =         0x02; // Does pin have a slave?
    static const uint8_t ICW_3_SLAVE =          0x04;
    static const uint8_t ICW_4 =                0x01; // Use 8086 PIC mode

    static const uint8_t PIC_MASK_INTERRUPTS =  0xff;

} // namespace kernel::cpu

