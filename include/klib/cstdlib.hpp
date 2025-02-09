/**
 * @file cstdlib.hpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief stdlib for cpp
 * @version 0.1
 * @date 2025-02-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <stdint.h>
#include <stddef.h>
#include <klib/stdlib.h>

#define MAX_NUM_STR_SIZE 65

template <typename T> static char* xtoa(T value, char* str, int base) {
    // Test base
    if (base < 2 || base > 16)
    {
        return NULL;
    }

    T convertedBase = static_cast<T>(base);

    // Check edge case of value == 0
    if (value == 0)
    {
        str[0] = '0';
        str[1] = '\0';
        return str;
    }
    

    T num = value;
    size_t index = 0;
    for (; num != 0; ++index)
    {
        char digit = static_cast<char>(num % convertedBase); // Guaranteed between 0 and base
        if (digit < 10)
        {
            str[index] = '0' + digit;
        }
        else
        {
            str[index] = 'A' + digit - 10;
        }
        num /= convertedBase;
    }

    if (convertedBase == 10 && value < 0)
    {
        str[index] = '-';
        index++;
    }

    // Terminate string
    str[index--] = '\0';

    // Now invert the string (it's the wrong way around)
    for (size_t i = 0; i < index/2+1; i++)
    {
         char tmp = str[i];
         str[i] = str[index-i];
         str[index-i] = tmp;
    }

    // Now we have everything
    return str;

}
