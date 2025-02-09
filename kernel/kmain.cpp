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


extern "C" {
    [[noreturn]] void kmain( uint32_t multiboot_flag,
            const struct multiboot_info_structure* info, uint32_t terminalIndex );
}

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

    out << "Terminal index is " << terminalIndex << "\n";
    out << "Multiboot flag is " << multiboot_flag << "\n";

    earlyPanic("Shouldn't be seeing this!!!");
}