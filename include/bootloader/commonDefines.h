/**
 * @file commonDefines.h
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Common definitions, mainly memory locations, mainly useful in assembly
 * @version 0.1
 * @date 2025-02-07
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#define _initial_mbr_address        0x7C00
#define _final_mbr_address          0x0800
#define _stack_pointer              0x7ff
#define _dap_memory_location        0x1000
#define _heap_location              0x1010

#define _disk_read_location         0x70000

#define _stage1_magic               0x3141

#define CODE32_SEGMENT              0x08
#define DATA32_SEGMENT              0x10
#define CODE16_SEGMENT              0x18
#define DATA16_SEGMENT              0x20