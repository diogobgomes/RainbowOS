/**
 * @file acpi.cpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Functions for finding the RSDP
 * @version 0.1
 * @date 2025-02-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <stdint.h>
#include <kernelInternal/acpiKernel.hpp>
#include <stddef.h>

// Auxiliary functions
static bool specialStrComp(const char*, const char*);
static kernel::acpi::xsdp_desc* searchRSDPDesc();
static bool checkSum(uint8_t* ptr, size_t size);
static bool checkRSDPDesc(kernel::acpi::xsdp_desc*);

// BUG replace this with a fucking memcmp, what crack were you smoking?
bool specialStrComp(const char* a, const char* b)
{
    for ( size_t i = 0; i < kernel::acpi::RSDP_SEARCH_STR_LEN; i++ )
    {
        if (a[i]!=b[i])
            return false;
    }
    return true;
}

kernel::acpi::xsdp_desc* searchRSDPDesc()
{
    using namespace kernel::acpi;
    
    char* ptr = reinterpret_cast<char*>(EBDA_START);
    // First search EBDA (16-byte aligned)
    for ( ; ptr < reinterpret_cast<char*>(EBDA_END - RSDP_SEARCH_STR_LEN);
            ptr++)//ptr+=16)
    {
        if (specialStrComp(RSDP_SEARCH_STR,ptr)) // Are the same
            return reinterpret_cast<xsdp_desc*>(ptr);
    }

    // Now search BIOS
    for ( ptr = reinterpret_cast<char*>(BIOS_START);
            ptr < reinterpret_cast<char*>(BIOS_END - RSDP_SEARCH_STR_LEN);
            ptr++)//ptr+=16 )
    {
        if (specialStrComp(RSDP_SEARCH_STR,ptr))
            return reinterpret_cast<xsdp_desc*>(ptr);
    }

    // Not found
    return nullptr;
}

bool checkSum(uint8_t* ptr, size_t size)
{
    unsigned int res = 0;
    for (size_t i = 0; i < size; i++)
    {
        res += ptr[i];
    }

    if( (res & 0xff) == 0)
        return true;
    return false;
}

bool checkRSDPDesc(kernel::acpi::xsdp_desc* ptr)
{
    if (ptr->revision == 0) // ACPI version 1.0
    {
        return checkSum(reinterpret_cast<uint8_t*>(ptr),sizeof(kernel::acpi::rsdp_desc));
    } else if (ptr->revision == 2) // ACPI version 2.0 and later
    {
        return checkSum(reinterpret_cast<uint8_t*>(ptr),sizeof(kernel::acpi::xsdp_desc));
    }

    // Invalid revision
    return false;
}

kernel::acpi::xsdp_desc* kernel::acpi::findRSDPDesc()
{
    using namespace kernel::acpi;

    xsdp_desc* ptr = searchRSDPDesc();

    if(!checkRSDPDesc(ptr))
        return nullptr;

    return ptr;
}