/**
 * @file stdlib.h
 * @author Diogo Gomes
 * @brief General purpose C library
 * @version 0.1
 * @date 2025-02-08
 * 
 */

#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Abort function, required by libgcc
 * 
 */
__attribute__((__noreturn__))
void abort(void);

/**
 * @brief Early panic function, simply prints "earlyPanic(): ", and str, to the
 * screen, and then hangs. In some configurations, it will change the background
 * to red to make it more visible
 * 
 * @param str String to print out
 */
[[noreturn,maybe_unused]] void earlyPanic(const char* str);

/**
 * @brief Hangs the computer (also has magic breakpoint for BOCHS)
 * 
 */
[[noreturn,maybe_unused]] inline static void hang()
{
    while(true)
    __asm__ __volatile__ (  "xchgw %bx, %bx\r\n"
                            "cli\r\n"
                            "hlt");
}


#ifdef __cplusplus
}
#endif
