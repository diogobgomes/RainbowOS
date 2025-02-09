/**
 * @file realMode.hpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Functions to run code in 16-bit real mode
 * @version 0.1
 * @date 2025-02-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <stdint.h>
#include <stddef.h>

extern "C" {
    void realModeCall(...);
}
