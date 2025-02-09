ENTRY(_start)
OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH("i386")
phys = 1M;

/* Tell linker where to put shit */
SECTIONS
{
    /* Where to begin putting sections */
    . = phys;

    /* Put the header first, and then the .text section */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot_header)
        *(.text)
    }

    /* Read-only data */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    /* Read-write data (initialized) */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    /* Read-write data (uninitialized) and stack */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}