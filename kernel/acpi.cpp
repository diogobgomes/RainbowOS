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
#include "acpiKernel.hpp"
#include <stddef.h>
//#include <klib/string.h>

// Auxiliary functions
static bool specialStrComp(const char*, const char*);
static kernel::acpi::xsdp* searchRSDP();
static bool checkSumOld(uint8_t*);
static bool checkSumNew(uint8_t*);
static bool checkRSDP(kernel::acpi::xsdp*);

bool specialStrComp(const char* a, const char* b)
{
    for ( size_t i = 0; i < kernel::acpi::RSDP_SEARCH_STR_LEN; i++ )
    {
        if (a[i]!=b[i])
            return false;
    }
    return true;
}

kernel::acpi::xsdp* searchRSDP()
{
    using namespace kernel::acpi;
    
    char* ptr = reinterpret_cast<char*>(EBDA_START);
    // First search EBDA (16-byte aligned)
    for ( ; ptr < reinterpret_cast<char*>(EBDA_END - RSDP_SEARCH_STR_LEN);
            ptr++)//ptr+=16)
    {
        if (specialStrComp(RSDP_SEARCH_STR,ptr)) // Are the same
            return reinterpret_cast<xsdp*>(ptr);
    }

    // Now search BIOS
    for ( ptr = reinterpret_cast<char*>(BIOS_START);
            ptr < reinterpret_cast<char*>(BIOS_END - RSDP_SEARCH_STR_LEN);
            ptr++)//ptr+=16 )
    {
        if (specialStrComp(RSDP_SEARCH_STR,ptr))
            return reinterpret_cast<xsdp*>(ptr);
    }

    // Not found
    return nullptr;
}

bool checkSumOld(uint8_t* ptr)
{
    unsigned int res = 0;
    for (size_t i = 0; i < sizeof(kernel::acpi::rsdp); i++)
    {
        res += ptr[i];
    }

    if( (res & 0xff) == 0)
        return true;
    return false;
}

bool checkSumNew(uint8_t* ptr)
{
    // Check old part
    if(!checkSumOld(ptr))
        return false;

    // Check new part
    unsigned int res = 0;
    for (size_t i = sizeof(kernel::acpi::rsdp); i < sizeof(kernel::acpi::xsdp); i++)
    {
        res += ptr[i];
    }

    if( (res & 0xff) == 0)
        return true;
    return false;
}

bool checkRSDP(kernel::acpi::xsdp* ptr)
{
    if (ptr->revision == 0) // ACPI version 1.0
    {
        return checkSumOld(reinterpret_cast<uint8_t*>(ptr));
    } else if (ptr->revision == 2) // ACPI version 2.0 and later
    {
        return checkSumNew(reinterpret_cast<uint8_t*>(ptr));
    }

    // Invalid revision
    return false;
}

kernel::acpi::xsdp* kernel::acpi::findRSDP()
{
    using namespace kernel::acpi;

    xsdp* ptr = searchRSDP();

    if(!checkRSDP(ptr))
        return nullptr;

    return ptr;
}