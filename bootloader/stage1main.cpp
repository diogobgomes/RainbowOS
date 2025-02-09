/**
 * @file bootloader/main.cpp
 * @author Diogo Gomes (dbarrosgomes@gmail.com)
 * @brief Main functions for the stage1 bootloader
 * @version 0.1
 * @date 2025-02-08
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include <stdint.h>
#include <stddef.h>
#include <klib/io.hpp>
#include <klib/cstdlib.hpp>
#include <devices/BIOSVideoIO.hpp>
#include <earlyLib/memory.hpp>
#include <earlyLib/diskRead16.hpp>
#include <earlyLib/memoryDetection16.hpp>
#include <bootloader/commonDefines.h>
#include <fs/fat32.hpp>
#include <fs/mbr.hpp>
#include <sys/elf.h>
#include <sys/multiboot.h>


/*******************************************************************************
 * 
 * DECLARATIONS
 * 
*******************************************************************************/
extern "C" {
    [[noreturn]] void _stage1Main(uint32_t dx, uint32_t __stop_stage1,
                            uint32_t cursor);
    [[noreturn]] void jumpKernel(uint32_t initAddr, uint32_t mbInfo,
                            uint32_t termIndex);
}

// Variables
// MBR is at _final_mbr_address
static fs::MBR *const mbr = reinterpret_cast<fs::MBR*>(_final_mbr_address);
io::_outstream<io::framebuffer_terminal> out;
static uint8_t disk;

// Tracemax stuff, we really should improve this
#ifdef TRACEMAX
    #include <klib/tracemax.hpp>
    tracemax<io::_outstream<io::framebuffer_terminal>> traceOut;
#endif

// Function declarations
int diskReadFunc( uint64_t LBA, void* buffer, size_t sectors );
uint32_t getPartitionLBA();
multiboot_header* multibootHeaderSearch(fs::fat32_fileResult* file);
multiboot_info_structure* buildMultibootInfo(multiboot_header* header,
        uint32_t bootDevice);
bool elf_check_file(elf32_Ehdr *header );
bool elf_check_prog_header(elf32_Phdr *header);
void* elf32_load(elf32_Ehdr *header );

/*******************************************************************************
 * 
 * FUNCTION DEFINITIONS
 * 
*******************************************************************************/

int diskReadFunc( uint64_t LBA, void* buffer, size_t sectors )
{
    return io::diskRead16(LBA,buffer,sectors,disk);
}

uint32_t getPartitionLBA()
{
    fs::partitionTable *ptr = reinterpret_cast<fs::partitionTable*>(
        reinterpret_cast<uint8_t*>(mbr) + 0x1be); // Point to first partition table
    for (size_t i = 0; i < 4; i++)
    {
        if (ptr[i].attributes & (1 << 7)) // Found first bootable disk
        {
            return ptr[i].lbaBegin;
        }
    }
    earlyPanic("getPartitionLBA(): Couldn't get the partition LBA!");
}

multiboot_header* multibootHeaderSearch(fs::fat32_fileResult* file)
{
    multiboot_header* header;

    for( header = reinterpret_cast<multiboot_header*>(file->ptr);
            // Search first n bytes, -12 for size of struct
            reinterpret_cast<uint8_t*>(header) <= 
                    reinterpret_cast<uint8_t*>(file->ptr) + MULTIBOOT_SEARCH - 12
            // Check for file size
            || reinterpret_cast<uint8_t*>(header) >=
                    reinterpret_cast<uint8_t*>(file->ptr) + file->size;
            header = reinterpret_cast<multiboot_header*>(
                    reinterpret_cast<uint8_t*>(header) + MULTIBOOT_HEADER_ALIGN))
    {
        if(header->magic == MULTIBOOT_MAGIC &&
                (header->magic + header->flags + header->checksum) == 0) // Found magic && checksum checks out
            return header;
    }

    return nullptr; // Didn't find it, return NULL
}

multiboot_info_structure* buildMultibootInfo(multiboot_header* header, uint32_t bootDevice)
{
    // Create the returnStruct
    auto returnStruct = new multiboot_info_structure;

    returnStruct->flags = 0;

    // Parse the header->flags field
    if (header->flags & 1) // Flags[0]
    {
        // Get lower memory size
        returnStruct->mem_lower = mem::queryLowerMemory();

        // Get upper memory
        auto upperMemStruct = mem::queryUpperMemory();
        returnStruct->mem_upper = mem::getUpperMemorySize(upperMemStruct); // If it's 0, mem is > 4 GiB

        returnStruct->mmap_length = upperMemStruct->size * 24; // All entries are 24 by default
        returnStruct->mmap_addr = reinterpret_cast<uint32_t>(upperMemStruct->ptr);

        returnStruct->flags = returnStruct->flags | 1;
        returnStruct->flags = returnStruct->flags | 1 << 6;
    }
    if (header->flags & 2) // Flags[1]
    {
        returnStruct->boot_device = bootDevice;
        returnStruct->flags = returnStruct->flags | 2;
    }
    if (header->flags & 4) // Flags[2]
    {
        return nullptr; // We cannot deal with this request (video mode)
    }
    if (header->flags & 65528) // Any other flags set until bit 15
    {
        return nullptr; // Multiboot compliance!!
    }

    return returnStruct;
}

bool elf_check_file(elf32_Ehdr *header ) {
    // Have to check each of the necessary parts of the header
    // Start by checking it's a valid pointer
    // TODO could actually separate in two functions, check if valid, and check if supported
    if(!header) return false;

    // Check magic numbers
    if(header->e_ident[EI_MAG0] != ELFMAG0) return false;
    if(header->e_ident[EI_MAG1] != ELFMAG1) return false;
    if(header->e_ident[EI_MAG2] != ELFMAG2) return false;
    if(header->e_ident[EI_MAG3] != ELFMAG3) return false;

    // If we got here, it's a valid ELF file, but we gotta check that we support it
    if(header->e_ident[EI_CLASS] != ELFCLASS32) return false;
    if(header->e_ident[EI_DATA] != ELFDATA2LSB) return false;
    if(header->e_machine != EM_386) return false;
    if(header->e_ident[EI_VERSION] != EV_CURRENT) return false;
    if(header->e_type != ET_EXEC) return false; // Kernel must be ET_EXEC

    // If we got here, it's a loadable kernel
    return true;
}

bool elf_check_prog_header(elf32_Phdr *header) {
    // Just check that the program header is compatible, no weird stuff
    // We can't deal with dynamic or interp
    if(header->p_type == PT_DYNAMIC) return false;
    if(header->p_type == PT_INTERP) return false;

    return true;
}

void* elf32_load(elf32_Ehdr *header )
{
    // ATTENTION: This makes no sanity checks on loadable segments, if they wanna be
    // loaded somewhere ridiculous, we're not checking, like, at all
    elf32_Phdr* ph = reinterpret_cast<elf32_Phdr*>
            (reinterpret_cast<uint8_t*>(header) + header->e_phoff);
    elf32_Phdr* eph = ph + header->e_phnum;

    size_t i = 0;

    for (; ph < eph; ph =  reinterpret_cast<elf32_Phdr*>
                (reinterpret_cast<uint8_t*>(ph) + header->e_phentsize))
    // Conversions inside for are necessary for pointer arithmetic to work
    // e_phnum is in bytes, but pointer arithmetic will assume size
    {
        out << "Found segment at 0x" << reinterpret_cast<uint32_t>(ph)
            << " of type " << ph->p_type << "\n";

        if( !(elf_check_prog_header(ph)) ) return 0; // We've hit something
        if( ph->p_type != PT_LOAD ) continue; // We don't care about these
        if( ph->p_memsz == 0 ) continue; // These are also not gonna be loaded
        uint8_t *dest = reinterpret_cast<uint8_t*>(ph->p_paddr);
        const uint8_t *src = reinterpret_cast<uint8_t*>
                (reinterpret_cast<uint32_t>(header) + ph->p_offset);
        
        // Copy it!
        out << "Loading it to 0x" << reinterpret_cast<uint32_t>(dest) << "\n";
        memcpy(dest,src,ph->p_filesz);

        // Check to see if p_memsz > p_filesz
        if(ph->p_memsz > ph->p_filesz) {
            // Fill with zeros
            memset(dest + ph->p_filesz, 0, ph->p_memsz - ph->p_filesz);
        }
        ++i;
    }
    // Set final memory address
    ph = reinterpret_cast<elf32_Phdr*>
            (reinterpret_cast<uint8_t*>(ph) - header->e_phentsize);
    void* finalMem = reinterpret_cast<void*>(ph->p_paddr + ph->p_memsz);

    out << "Loaded " << i << " segments total!\n";
    return finalMem;
}

void _stage1Main( uint32_t dx, uint32_t __stop_stage1, uint32_t cursor)
{
    // Initialize the terminal
    io::framebuffer_terminal terminal;
    out.init(&terminal);
    unsigned int row = cursor >> 8;
    unsigned int column = cursor & 0xff00;
    terminal.setCursor(row,column);
    out.hex();        

    // Print the header
    terminal.setColor(io::vga_color::VGA_COLOR_LIGHT_GREY,io::vga_color::VGA_COLOR_BLUE);
    out << "Welcome to RainbowOS 32-bit protected mode bootloader!\n";
    terminal.setColor(io::vga_color::VGA_COLOR_LIGHT_GREY,io::vga_color::VGA_COLOR_BLACK);

    // TRACEMAX stuff
    #ifdef TRACEMAX
        traceOut.init(&out);
        traceOut << "TRACEMAX is active\n";
    #endif


    // Initialize the heap
    const uint32_t heapMaxSize = _disk_read_location - __stop_stage1;
    mem::heapInitialize(reinterpret_cast<void*>(__stop_stage1),heapMaxSize);

    // Reading files from disk
    uint32_t activePartitionLBA = getPartitionLBA();
    disk = uint8_t(dx); // Initialize global disk variable

    fs::fat32 activePartition((&diskReadFunc));
    activePartition.init(activePartitionLBA);

    #ifdef TRACEMAX
        traceOut << "Reading KERNEL.BIN\n";
    #endif
    fs::fat32_fileResult* kernelFile = activePartition.getRootFile("KERNEL.BIN");

    //FIXME This fat implementation is horrible
    if (kernelFile->returnCode == 1)
    {
        earlyPanic("Could not find KERNEL.BIN");
    }

    #ifdef TRACEMAX
        traceOut << "Read file KERNEL.BIN, size is: " << kernelFile->size << 
            ", and ptr is " << reinterpret_cast<uint32_t>(kernelFile->ptr) << "\n";
    #endif

    // Find multiboot header
    multiboot_header* mbHeader = multibootHeaderSearch(kernelFile);
    out << "mbHeader = " << reinterpret_cast<uint32_t>(mbHeader) << "\n";
    if(! mbHeader) earlyPanic("Couldn't find Multiboot Header!");

    // Start building the multiboot info table
    multiboot_info_structure* mbInfo = buildMultibootInfo(mbHeader, disk);

    // Now ELF load init.bin
    elf32_Ehdr *kernelElfHeader = reinterpret_cast<elf32_Ehdr*>(kernelFile->ptr);
    if (!(elf_check_file(kernelElfHeader)))
        earlyPanic("init.bin's ELF header is not recognized, or not supported!");

    void* initEndPtr = elf32_load(kernelElfHeader);

    if (! initEndPtr )
        earlyPanic("Problems loading ELF binary kernel.bin");

    //BUG This should be good to remove
    // Setup modules in mbInfo
    /*auto kernelModEntry = new multiboot_mod_list;
    const char kernelNameStr[] = "PATROCLUS_KERNEL";
    kernelModEntry->mod_start = reinterpret_cast<uint32_t>(kernelFile->ptr);
    kernelModEntry->mod_end = reinterpret_cast<uint32_t>(kernelFile->ptr) + kernelFile->size;
    kernelModEntry->cmdline = reinterpret_cast<uint32_t>(kernelNameStr);
    kernelModEntry->pad = 0;

    mbInfo->flags = mbInfo->flags | 1 << 3; // Say we have modules
    mbInfo->mods_count = 1;
    mbInfo->mods_addr = reinterpret_cast<uint32_t>(kernelModEntry);*/

    // Setup for jump
    out << "Everything seems to be good, jumping to init\n";
    uint32_t index = terminal.getRow() * terminal.vga_width + terminal.getColumn();
    uint32_t kernelAddr = kernelElfHeader->e_entry;
    uint32_t mbInfoPtr = reinterpret_cast<uint32_t>(mbInfo);

    jumpKernel(kernelAddr,mbInfoPtr,index);

    earlyPanic("Should never get here, something is wrong!!");
    __asm__ __volatile__ ("cli\r\nhlt");
    __builtin_unreachable();
}

