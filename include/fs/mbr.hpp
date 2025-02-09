/**
 * @file mbr.hpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Structs for reading the MBR
 * @version 0.1
 * @date 2025-02-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <stdint.h>

namespace fs
{

struct partitionTable
{
    /* Drive attributes (bit 7 set = bootable) */
    uint8_t         attributes;

    /* CHS Address of partition start */
    uint8_t         chsStart[3];

    /* Partition Type */
    uint8_t         partType;

    /* CHS Address of last partition sector */
    uint8_t         chsEnd[3];

    /* LBA of partition start */
    uint32_t        lbaBegin;

    /* Number of sectors in partition */
    uint32_t        numSectors;
} __attribute__((packed)) ;

static_assert(sizeof(partitionTable) == 16, "partitionTable struct is too big!");
    
struct MBR
{
    /* Bootstrap code, flat binary */
    uint8_t         bootstrap[440];

    /* Unique disk ID, it's actually optional */
    uint32_t        uniqueID;

    /* Reserved */
    uint16_t        reserved;

    /* Partition Tables */
    partitionTable  part1;
    partitionTable  part2;
    partitionTable  part3;
    partitionTable  part4;

    /* "Valid bootsector" signature bytes, should be 0x55AA */
    uint16_t        signature;
} __attribute__((packed)) ;


static_assert(sizeof(MBR) == 512, "MBR struct is too big!");

} // namespace fs
