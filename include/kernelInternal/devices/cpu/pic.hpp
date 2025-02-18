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
    static const uint8_t ICW1_INIT =            0x10; // Initialization
    static const uint8_t ICW1_LEVEL =           0x08; // Level triggered (edge) mode
    static const uint8_t ICW1_INTERVAL4 =       0x04; // Call address interval 4
    static const uint8_t ICW1_SINGLE =          0x02; // Single (cascade) mode
    static const uint8_t ICW1_ICW4 =            0x01; // ICW4 will be present

    static const uint8_t ICW4_8086 =            0x01; // 8086 mode
    static const uint8_t ICW4_AUTO =            0x02; // Auto EOI
    static const uint8_t ICW4_BUF_SLAVE =       0x08; // Buffered mode/slave
    static const uint8_t ICW4_BUF_MASTER =      0x0c; // Buffered mode/master
    static const uint8_t ICW4_SFNM =            0x10; // Special fully nested

    static const uint8_t PIC_MASK_INTERRUPTS =  0xff;

} // namespace kernel::cpu

