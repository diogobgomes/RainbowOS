/**
 * @file diskRead16.cpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Definitions from earlyLib/diskRead16.hpp
 * @version 0.1
 * @date 2025-02-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <earlyLib/diskRead16.hpp>
#include <stddef.h>
#include <stdint.h>
#include <bootloader/commonDefines.h>
#include <klib/string.h>
#include <earlyLib/realMode.hpp>
#include <klib/cstdlib.hpp>
//#include <klib/tracemax.hpp>

static io::DAP *dap = reinterpret_cast<io::DAP*>(_dap_memory_location);
static constexpr uint32_t readBuffer = _disk_read_location;
static constexpr uint32_t offset = readBuffer % 16;
static constexpr uint32_t segment = (readBuffer - offset) / 16;


static void _fillDAP(uint32_t LBA, size_t sectors)
{
    dap->size = 0x10;
    dap->unused = 0;
    dap->numSectors = uint16_t(sectors);
    dap->buffer = (segment << 16) | offset;
    dap->LBA = LBA;
    return;
}


bool io::diskRead16(uint32_t LBA, void* buffer, size_t sectors, uint16_t disk)
{
    const size_t iterations = sectors / MAX_SECTORS;
    void* currentBuffer = buffer;
    uint32_t currentLBA = LBA;

    for (size_t i = 0; i <= iterations; i++)
    {
        size_t currentSectors = i == iterations ? sectors - MAX_SECTORS*iterations: MAX_SECTORS;

        _fillDAP(currentLBA,currentSectors);
        
        uint8_t result = 0;

        realModeCall(&asmCall_int13,static_cast<uint32_t>(disk), reinterpret_cast<uint32_t>(&result));

        if(result) earlyPanic("diskRead16(): Failure reading");

        // No error, move it
        memmove(currentBuffer,reinterpret_cast<void*>(readBuffer),currentSectors*SECTOR_SIZE);

        // Buffer, lba
        currentBuffer = reinterpret_cast<void*>(
            reinterpret_cast<uint32_t>(currentBuffer) + MAX_SECTORS*SECTOR_SIZE);
        currentLBA += MAX_SECTORS;
    }
    return true;
    
}