/**
 * @file acpi.cpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Definitions from acpi.hpp
 * @version 0.1
 * @date 2025-02-16
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <kernelInternal/acpi.hpp>
#include <kernelInternal/acpiKernel.hpp>
#include <klib/cstdlib.hpp>
#include <klib/string.h>

using namespace kernel::acpi;

/**========================================================================
 *                           CLASS acpi_header
 *========================================================================**/
acpi_header::acpi_header()
{
    // First of all, we must try to find the xsdp_desc, and abort if we can't
    auto RSDPDescPtr = findRSDPDesc();
    if (RSDPDescPtr == nullptr)
        earlyPanic("In kernel::acpi::apicHeader() constructor: Error: Could not find the XSDP, aborting");
    
    // Now, see if we're dealing with a type 1 or 2 acpi
    _type2 = RSDPDescPtr->revision == 2 ? true : false;

    // Set ptr to the thing
    _ptr = RSDPDescPtr;
}

acpi_sdt_header* acpi_header::findEntry(const char* name)
{
    // First, validate input
    if(strlen(name) != 4)
        earlyPanic("In kernel::acpi::apicHeader::findEntry(): Error: Input string has invalid size");
    
    // Gloval variables
    size_t index = 0;
    size_t numEntries;

    if (_type2 && (_ptr->xsdt_address != 0)) // Type 2, use XSDP
    {
        auto xsdtHeader = reinterpret_cast<xsdt*>(_ptr->xsdt_address);
        numEntries = 
            (xsdtHeader->sdt_header.length - sizeof(acpi_sdt_header)) / 8;
        
        for(; index < numEntries; index++ )
        {
            auto tempPtr = reinterpret_cast<acpi_sdt_header*>
                (xsdtHeader->sdt_addresses[index]);
            auto tempStr = tempPtr->signature;
            if(memcmp(tempStr,name,4) == 0) // Found it
                return tempPtr;
        }
        // No entry was found, return nullptr
        return nullptr;
    }
    else // Type 1, use RSDT
    {
        auto rsdpHeader = reinterpret_cast<rsdp*>(_ptr->rsdt_address);
        numEntries =
            (rsdpHeader->sdt_header.length - sizeof(acpi_sdt_header)) / 4;

        for(; index < numEntries; index++ )
        {
            auto tempPtr = reinterpret_cast<acpi_sdt_header*>
                (rsdpHeader->sdt_addresses[index]);
            auto tempStr = tempPtr->signature;

            if(memcmp(tempStr,name,4) == 0) // Found it
                return tempPtr;
        }
        // No entry was found, return nullptr
        return nullptr;
    }
}

int acpi_header::getType()
{
    return _type2 ? 2 : 1;
}

/**========================================================================
 *                           CLASS acpi_madt
 *========================================================================**/

acpi_madt::acpi_madt(acpi_header* acpiPtr)
{
    // Try to find MADT entry
    auto foundPtr = acpiPtr->findEntry("APIC"); // APIC is keyword for MADT
    if(foundPtr == nullptr)
        earlyPanic("In kernel::acpi::acpiMadt constructor: Error: No MADT was found");
    _ptr = reinterpret_cast<madt_table*>(foundPtr);
}

void* acpi_madt::getEntry(int type)
{
    // For now, we only support type 1
    if (type != 1)
    {
        return nullptr;
    }

    // Walk through the fields
    auto walker = reinterpret_cast<uint8_t*>(_ptr) + sizeof(madt_table);
    auto limit = reinterpret_cast<uint8_t*>(_ptr) + _ptr->header.length;
    while (walker < limit)
    {
        auto curr = reinterpret_cast<madt_entry_header*>(walker);
        if (curr->entry_type == 1)
            return reinterpret_cast<void*>(walker);
        // Walk to next one        
        walker += curr->record_length;
    }
    
    // If we got here, we found nothing
    return nullptr;
}