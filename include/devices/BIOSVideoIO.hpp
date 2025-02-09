/**
 * @file framebuffer_io.hpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Defines classes for accessing memory-mapped framebuffer IO
 * @version 0.1
 * @date 2025-02-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once
#include <stdint.h>
#include <stddef.h>

namespace io
{

constexpr uint32_t FRAMEBUFFER_LOCATION = 0xb8000;

/**
 * @brief Possible colors for framebuffer io
 * 
 */
enum class vga_color:uint8_t {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};


class framebuffer_terminal
{
public:
    /**
     * @brief Class constructor, sets up the terminal, but doesn't clear it
     * 
     */
    framebuffer_terminal() : _row(0), _column(0),_color(0x07), _fullLine(false),
                _buffer(reinterpret_cast<uint16_t*>(FRAMEBUFFER_LOCATION)) {} ;

    /**
     * @brief Resets the terminal, like if the constructor had been called
     * 
     */
    void reset() {
        _buffer = reinterpret_cast<uint16_t*>(FRAMEBUFFER_LOCATION);
        _row = 0;
        _column = 0;
        _fullLine = false;
        setColor(vga_color::VGA_COLOR_LIGHT_GREY,vga_color::VGA_COLOR_BLACK);
    }

    /**
     * @brief Clears the terminal with the currently set color
     * 
     */
    void clear();

    /**
     * @brief Set the Color of the terminal
     * 
     * @param fg Foreground color
     * @param bg Background color
     */
    void setColor(enum vga_color fg, enum vga_color bg);

    bool setCursor(uint8_t row, uint8_t column);

    uint16_t getRow() {return _row;}

    uint16_t getColumn() {return _column;}

    /**
     * @brief Scrolls the terminal by lines
     * 
     * @param lines Number of lines to scroll
     * @return int 0 for success, any other number for failure
     */
    int scroll(size_t lines);
    
    const size_t vga_height = 25;
    const size_t vga_width = 80;

    /**
     * @brief Puts a character in the next available location and increments position
     * 
     * @param c Character to put
     * @return int 0 for success, 1 for character out-of-bounds
     */
    int putchar(char c);

    

private:
    uint16_t _row;
    uint16_t _column;
    uint8_t _color;
    bool _fullLine; // Ugly hack, to prevent problem with full line and a '\n'
    uint16_t* _buffer;

    uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
        return static_cast<uint8_t>(fg) | static_cast<uint8_t>(bg) << 4;
    }

    uint16_t vga_entry(unsigned char uc, uint8_t tColor) {
        return static_cast<uint16_t>(uc) | tColor << 8;
    }

}; // class framebuffer_terminal

class realMode_terminal
{
public:
void reset() {
    /* %ah = 0 -> set the video mode
       %al = 0x3 -> Video mode = colour, 80*25 Text */
    __asm__ __volatile__ ("int $0x10" : : "a"(0x0003));
}

void clear() {
    /* %ah = 0x06 -> Scroll up window
       %al = 0x0 -> Clear
       %bh = 0x08 -> Background black, foreground light_grey
       %ch = 0, %cl = 0 -> 0x0 upper corner
       %dh = 24, %dl = 79 -> lower bottom corner
       Clobblers %dx and %cx : using dummy variables to track that */
    uint8_t c,d;
    __asm__ __volatile__ ("int $0x10"
                            : "=c"(c), "=d"(d)
                            : "a"(0x0600), "b"(0x0800), "c"(0), "d"(0x184f));
}

int scroll(size_t lines) {
    if(lines == 0) return 1; // Not allowed
    /* %ah = 0x06 -> Scroll up window
       %al = lines -> Number of lines
       %bh = 0x08 -> Background black, foreground light_grey
       %ch = 0, %cl = 0 -> 0x0 upper corner
       %dh = 24, %dl = 79 -> lower bottom corner*/
    __asm__ __volatile__ ("int $0x10"
                            :
                            : "a"(0x0600 | static_cast<uint8_t>(lines)), "b"(0x0800), "c"(0), "d"(0x184f) );
    return 0;
}

int putchar(char c) {
    /* %ah = 0x0e -> Teletype mode
       %al = c -> Character*/
    //TODO Add character bound detection
    __asm__ __volatile__ ("int $0x10"
                            :
                            : "a"(0x0e00 | c), "b"(0));
    return 0;
}


}; // class realMode_terminal





} // namespace framebuffer_io
