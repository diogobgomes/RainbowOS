/**
 * @file string.c
 * @author Diogo Gomes
 * @brief Implementation of very basic functions, memcpy, memcmp, memmove, memset,
 * strlen, to be used by the bootloader/kernel
 * @version 0.1
 * @date 2025-02-08
 * 
 */

#include <stdint.h>
#include <stddef.h>
#include <klib/string.h>

size_t strlen(const char* str) 
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	for (size_t i = 0; i < size; i++)
		dst[i] = src[i];
	return dstptr;
}

void* memset(void* bufptr, int value, size_t size) {
	unsigned char* buf = (unsigned char*) bufptr;
	for (size_t i = 0; i < size; i++)
		buf[i] = (unsigned char) value;
	return bufptr;
}

int memcmp(const void* ptr1, const void* ptr2, size_t size) {
	const unsigned char* a = (const unsigned char*) ptr1;
	const unsigned char* b = (const unsigned char*) ptr2;
	for (size_t i = 0; i < size; i++) {
		if (a[i] < b[i])
			return -1;
		else if (b[i] < a[i])
			return 1;
	}
	return 0;
}

void* memmove(void* dstptr, const void* srcptr, size_t size) {
	unsigned char* dst = (unsigned char*) dstptr;
	const unsigned char* src = (const unsigned char*) srcptr;
	if (dst < src) {
		for (size_t i = 0; i < size; i++)
			dst[i] = src[i];
	} else {
		for (size_t i = size; i != 0; i--)
			dst[i-1] = src[i-1];
	}
	return dstptr;
}

int strcmp(const char* str1, const char* str2)
{
	size_t i = 0;
	for ( ; str1[i] != '\0' && str2[i] != '\0'; i++)
	{
		if (str1[i] != str2[i])
		{
			return str1[i] - str2[i];
		}
	}

	return str1[i] - str2[i];	
}

char* strcpy(char* dest, const char* src)
{
	size_t i = 0;
	do
	{
		dest[i] = src[i];
	} while (src[i++] != '\0');
	
	return dest;
}