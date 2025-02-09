/**
 * @file elf.h
 * @author Diogo Gomes
 * @brief Definitions for ELF header and other useful, ELF related things
 * @version 0.1
 * @date 2023-11-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Macros for handling definition of inline functions
#ifndef INLINE
# define INLINE static inline
#endif



/* HEADER STUFF */
/* CONSTANTS */
/* Size of ELF identification field */
#define ELF_NIDENT      16

/* ELF magic numbers */
#define ELFMAG0         0x7f
#define ELFMAG1         'E'
#define ELFMAG2         'L'
#define ELFMAG3         'F'

#define ELFDATA2LSB     (1) // Little endian
#define ELFCLASS32      (1) // 32-bit architecture
#define ELFCLASS64      (2) // 64-bit architecture

/* ELF file types */
#define ET_NONE         (0) // No file type
#define ET_REL          (1) // Relocatable file
#define ET_EXEC         (2) // Executable file
#define ET_DYN          (3) // Shared object file
#define ET_CORE         (4) // Core file
#define ET_LOPROC       (0xff00) // Processor specific
#define ET_HIPROC       (0xffff) // Processor specific

/* Machine type */
#define EM_386          (3) // Intel 80386
#define EM_X86_64       (62) // AMD x86-64

/* Versions */
#define EV_NONE         (0) // Invalid version
#define EV_CURRENT      (1) // Current version

/**
 * @brief ELF32 header structure, present at the beginning of every file
 * 
 */
typedef struct
{
    /* ELF identification field, always equal for all ELF file, access through elf_Ident type*/
    uint8_t             e_ident[ELF_NIDENT];

    /* Identifies the object file type, for kernel should be EXEC */
    uint16_t            e_type;

    /* Specifies the required architecture of an individual file, should be Intel 80386 */
    uint16_t            e_machine;
    
    /* Identifies the object file version */
    uint32_t            e_version;

    /* Program entry point, to which the loader transfers control */
    uint32_t            e_entry;

    /* Holds the program header table's file offset in bytes, 0 for no table */
    uint32_t            e_phoff;

    /* Holds the section header table's file offset in bytes, 0 for no table */
    uint32_t            e_shoff;

    /* Processor-specific flags associated with the file */
    uint32_t            e_flags;

    /* ELF header size in bytes */
    uint16_t            e_ehsize;

    /* Size in bytes of entries in the file's program header table */
    uint16_t            e_phentsize;

    /* Number of entries in the program header table, 0 if no table */
    uint16_t            e_phnum;

    /* Size in bytes of section headers */
    uint16_t            e_shentsize;

    /* Number of entries in the section header table */
    uint16_t            e_shnum;

    /* Section header table index of the entry associated with the section name string table. SHN_UNDEF if no section */
    uint16_t            e_shstrndx;

} elf32_Ehdr ;

/**
 * @brief Indent for access to the ELF header e_ident
 * 
 */
enum elf_Ident {
    EI_MAG0 =           0, // 0x7f
    EI_MAG1 =           1, // 'E'
    EI_MAG2 =           2, // 'L'
    EI_MAG3 =           3, // 'F'
    EI_CLASS =          4, // Architecture (32/64), 1 for 32-bit
    EI_DATA =           5, // Byte Order
    EI_VERSION =        6, // ELF Version, must be EV_CURRNT
    EI_OSABI =          7, // OS Specific
    EI_ABIVERSION =     8, // OS Specific
    EI_PAD =            9 // Padding
};

// TODO add info on section table
// For now, we don't care about it


/* PROGRAM HEADER STUFF */
/* CONSTANTS */
/* Segment types */
#define PT_NULL         0 // Array element is unused
#define PT_LOAD         1 // Loadable segment, described by p_filesz and p_memsz
                          // If p_memsz > p_filesz, first have the initialized area, and then the uninitialized, filed with zeros
#define PT_DYNAMIC      2 // Specifies dynamic linking information
#define PT_INTERP       3 // Path to interpreter
#define PT_NOTE         4 // Specifies location and size of auxiliary information
#define PT_SHLIB        5 // Reserved, unspecified
#define PT_PHDR         6 // Location and size of the program header table itself, not usefull for us
#define PT_LOPROC       0x70000000 // Processor specific semantics
#define PT_HIPROC       0x7fffffff

/**
 * @brief Program header (32-bit), defines information about how to load the ELF into memory
 * 
 */
typedef struct
{
    /* What kind of segment this array element describes */
    uint32_t            p_type;

    /* Offset from the beginning of the file at which the first byte of this resides */
    uint32_t            p_offset;

    /* Virtual address at which the first byte of the segment resides in memory */
    uint32_t            p_vaddr;

    /* Segment's physical address (usually ignored) */
    uint32_t            p_paddr;

    /* Number of bytes in the file image that the segment takes up, may be 0 */
    uint32_t            p_filesz;

    /* Number of bytes in the memory image of the segment, may be 0 (.bss will have memsize != filesize) */
    uint32_t            p_memsz;

    /* Flags relevant to the segment */
    uint32_t            p_flags;

    /* Program alignment requirements */
    uint32_t            p_align;
} elf32_Phdr;


#ifdef __cplusplus
}
#endif

