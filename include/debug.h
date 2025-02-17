/**
 * @file debug.h
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Useful macros for debugging
 * @version 0.1
 * @date 2025-02-17
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#define BOCHS_STOP __asm__ __volatile__ ("xchgw %bx, %bx");

