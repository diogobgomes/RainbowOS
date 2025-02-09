/**
 * @file bootloader/diskRead16.hpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Functions to read stuff from disk using int 13h in real mode
 * @version 0.1
 * @date 2025-02-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <stddef.h>
#include <stdint.h>

#define MAX_SECTORS 127
#define SECTOR_SIZE 512

namespace io
{

struct DAP
{
    uint8_t size;
    uint8_t unused;
    uint16_t numSectors;
    uint32_t buffer;
    uint64_t LBA;
};


bool diskRead16(uint32_t LBA, void* buffer, size_t sectors, uint16_t disk);


} // namespace io

extern "C" void asmCall_int13( uint32_t disk, uint32_t resultPtr );