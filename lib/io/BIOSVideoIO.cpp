/**
 * @file BIOSVideoIO.cpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Definitions from BIOSVideoIO.hpp
 * @version 0.1
 * @date 2025-02-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <stdint.h>
#include <stddef.h>
#include <klib/string.h>
#include <devices/BIOSVideoIO.hpp>

void io::framebuffer_terminal::clear()
{
    for (size_t y = 0; y < vga_height; y++)
    {
        for (size_t x = 0; x < vga_width; x++)
        {
            const size_t index = y * vga_width + x;
            _buffer[index] = vga_entry(' ',_color);
        }
        
    }
}

void io::framebuffer_terminal::setColor(enum vga_color fg, enum vga_color bg)
{
    _color = vga_entry_color(fg,bg);
}

int io::framebuffer_terminal::putchar(char c)
{
    if(c=='\n')
    {
        // Edge case where last line was full, and we sent a newline
        if (_fullLine)
        {
            _fullLine = false;
            return 0;
        }
        
        _column = 0;
        if (++_row == vga_height)
        {
            scroll(1);
            _row--;
        }
        return 0;
    }

    _fullLine = false; // Clear potential full line
    
    const size_t index = _row* vga_width + _column;

    // Check the character
    if (c < 20 || c > 126)
    {
        return 1; // Failure, character out of bounds
    }

    _buffer[index] = vga_entry(static_cast<unsigned char>(c), _color);

    // Increment column, check if we need to change lines, or scroll
    if (++_column == vga_width)
    {
        _column = 0;
        _fullLine = true;
        if (++_row == vga_height)
        {
            scroll(1);
            _row--;
        }
    }

    return 0;
    
    
}

int io::framebuffer_terminal::scroll(size_t lines)
{
    // Check lines is number allowed
    if (lines >= vga_height)
    {
        return 1;
    }

    size_t size = vga_width * (vga_height - lines) * 2;

    memmove(_buffer, _buffer + vga_width*lines, size);

    // Clear last lines
    for (size_t y = vga_height - lines; y < vga_height; y++)
    {
        for (size_t x = 0; x < vga_width; x++)
        {
            const size_t index = y * vga_width + x;
            _buffer[index] = vga_entry(' ',_color);
        }   
    }

    return 0;
}

bool io::framebuffer_terminal::setCursor(uint8_t row, uint8_t column)
{
    if (row >= vga_height || column >= vga_width)
    {
        return false;
    }

    _row = row;
    _column = column;
    return true;
    
}
