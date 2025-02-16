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

 //BUG Move this to acpi.hpp

#pragma once

#include <stdint.h>
#include <kernelInternal/acpi.hpp>

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

/**
 * @brief Finds the RSDP (XSDP) ACPI tables in the appropriate memory locations
 * 
 * @return xsdp_desc* Pointer to found table, nullptr if no table is found
 */
xsdp_desc* findRSDPDesc(); // BUG in the validation we're doing it all wrong

} // namespace kernel::acpi
