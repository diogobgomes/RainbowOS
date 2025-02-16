/**
 * @file acpi.hpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Definitions to deal with the ACPI
 * @version 0.1
 * @date 2025-02-14
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#pragma once

#include <stdint.h>

namespace kernel::acpi
{
    
struct rsdp_desc
{
    char            signature[8];
    uint8_t         checksum;
    char            OEMID[6];
    uint8_t         revision;
    uint32_t        rsdt_address;
}__attribute__((packed));

struct xsdp_desc
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

struct acpi_sdt_header
{
    char            signature[4];
    uint32_t        length;
    uint8_t         revision;
    uint8_t         checksum;
    char            oemid[6];
    char            oemt_table_id[8];
    uint32_t        oem_revision;
    uint32_t        creator_id;
    uint32_t        creator_revision;
}__attribute__((packed));

struct rsdp
{
    acpi_sdt_header sdt_header; // Signature "RSDP"
    uint32_t        sdt_addresses[];
}__attribute__((packed));

struct xsdt
{
    acpi_sdt_header sdt_header; // Signature "XSDT"
    uint64_t        sdt_addresses[];
}__attribute__((packed));

struct madt_table
{
    acpi_sdt_header header;
    uint32_t        lapic_address;
    /* Flag field. 1 means that dual 8259 legacy PICs installed */
    uint32_t        flags;
}__attribute__((packed));

struct madt_entry_header
{
    uint8_t         entry_type;
    uint8_t         record_length;
}__attribute__((packed));

struct madt_entry_type0
{
    madt_entry_header       header;
    uint8_t                 apic_processor_id;
    uint8_t                 apic_id;
    uint32_t                flags;
}__attribute__((packed));

struct madt_entry_type1
{
    madt_entry_header       header;
    uint8_t                 io_apic_id;
    uint8_t                 reserved;
    uint32_t                io_apic_address;
    uint32_t                global_system_interrupt_base;
}__attribute__((packed));

class acpi_header
{
private:
    xsdp_desc* _ptr;
    bool _type2; // Is it a version 2.0+ ACPI or only version 1.0

public:
    acpi_header();

    /**
     * @brief Search for entry of name
     * 
     * @param name Name of entry, must be a length 4 string
     * @return xsdt* Found entry, nullptr if nothing was found
     */
    acpi_sdt_header* findEntry(const char* name);
    int getType();
};

class acpi_madt
{
//private:
public:
    madt_table* _ptr;
//    bool _type2;
//public:
    acpi_madt(acpi_header*);

    void* getEntry(int type);

    //size_t getNumberInputs();
};

} // namespace kernel::acpi
