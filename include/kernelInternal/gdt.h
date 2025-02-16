/**
 * @file gdt.h
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Basic definitions of selectors for the GDT
 * @version 0.1
 * @date 2025-02-10
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Stuff for the GDT
#define CODE32_KSEGMENT              0x08
#define DATA32_KSEGMENT              0x10
#define CODE32_USEGMENT              0x18
#define DATA32_USEGMENT              0x20

#ifdef __cplusplus
}
#endif