/**
 * @file acpi.hpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Contains definitions for dealing with the ACPI
 * @version 0.1
 * @date 2025-02-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <stdint.h>

namespace kernel::acpi
{

// CONSTANTS
static const int EBDA_START =            0x80000;
static const int EBDA_END =              0x9ffff;
static const int BIOS_START =            0xe0000;
static const int BIOS_END =              0xfffff;

static const char RSDP_SEARCH_STR[] =    "RSD PTR";
static const int RSDP_SEARCH_STR_LEN =   7; /* We could do constexpr, but would have
                                        to implement function, it's just easier*/


struct rsdp
{
    char            signature[8];
    uint8_t         checksum;
    char            OEMID[6];
    uint8_t         revision;
    uint32_t        rsdt_address;
}__attribute__((packed));

struct xsdp
{
    char            signature[8];
    uint8_t         checksum;
    char            oemid[6];
    uint8_t         revision;
    uint32_t        rsdt_address;

    uint32_t        length;
    uint64_t        xsdt_address;
    uint8_t         extended_checksumn;
    uint8_t         reserverd[3];
}__attribute__((packed));

/**
 * @brief Finds the RSDP (XSDP) ACPI tables in the appropriate memory locations
 * 
 * @return xsdp* Pointer to found table, nullptr if no table is found
 */
xsdp* findRSDP();

} // namespace kernel::acpi
