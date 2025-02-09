/**
 * @file string.h
 * @author Diogo Gomes
 * @brief Header file for string.c
 * @version 0.1
 * @date 2025-02-08
 * 
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Macros for handling definition of inline functions
#ifndef INLINE
# define INLINE static inline
#endif

/**
 * @brief Get the size of a C-terminated string
 * 
 * @param str String to get size of
 * @return size_t Size of string
 */
size_t strlen(const char* str);

/**
 * @brief Copy memory
 * 
 * @param dstptr Pointer to destination
 * @param srcptr Pointer to source
 * @param size Size of memory to copy
 * @return void* Returns dstptr
 */
void* memcpy(void* __restrict dstptr, const void* __restrict srcptr, size_t size);

/**
 * @brief Set a region of memory to a specific value
 * 
 * @param bufptr Pointer to region of memory to set
 * @param value Value to set
 * @param size Size of memory to set
 * @return void* Returns bufptr
 */
void* memset(void* bufptr, int value, size_t size);

/**
 * @brief Compare two memory locations
 * 
 * @param ptr1 Pointer to memory location
 * @param ptr2 Pointer to memory location
 * @param size Size of memory to be compared
 * @return int Less than 0 for &ptr1 < &ptr2, more than 0 for &ptr1 > &ptr2, and
 * 0 for &ptr1 = &ptr2
 */
int memcmp(const void* ptr1, const void* ptr2, size_t size);

/**
 * @brief Copy memory, but for overlapping memory blocks, also works. Basically
 * a better memcpy
 * 
 * @param dstptr Pointer to destination
 * @param srcptr Pointer to source
 * @param size Size of memory to move
 * @return void* Returns dstptr
 */
void* memmove(void* dstptr, const void* srcptr, size_t size);

/**
 * @brief Compare two strings
 * 
 * @param str1 
 * @param str2 
 * @return int 0 if they are equal, >0 if first non-matching character is greater in str1, and <0 for opposite
 */
int strcmp(const char* str1, const char* str2);

/**
 * @brief Copy src to dest, including the terminating char, and stopping there
 * 
 * @param src 
 * @param dest 
 * @return char* Returns dest
 */
char* strcpy(char* dest, const char* src);

#ifdef __cplusplus
}
#endif

