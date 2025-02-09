/**
 * @file abort.c
 * @author Diogo Gomes
 * @brief Abort implementation, required by libgcc
 * @version 0.1
 * @date 2025-02-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <klib/stdlib.h>
 
__attribute__((__noreturn__))
void abort(void) {
	// TODO: Add proper kernel panic.
	earlyPanic("kernel: panic: abort()");
    __asm__ __volatile__("cli;"
                 "hlt;");

	while (1) { }
	__builtin_unreachable();
}