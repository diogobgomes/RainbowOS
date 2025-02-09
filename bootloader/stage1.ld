/* Stage1 linker script - v0.1

    
    2025 Diogo Gomes */

ENTRY(_start)
OUTPUT_FORMAT("binary")
OUTPUT_ARCH("i386")
phys = 0x1010;

SECTIONS
{

    . = phys;

    .text : {
        KEEP(*(.header))
        /*KEEP(*(.entry))*/
        /*KEEP(CMakeFiles/stage1.bin.dir/stage1.S.obj(.text))*/

        /*stage1.S.obj(.text);*/
        . = ALIGN(4);
        *(.text)
        *(.text.*)
        . = ALIGN(4);
        *(.rodata)
		*(.rodata.*)
		*(.data)
		*(.data.*)
		*(.bss)
		*(.bss.*)
    }


    /*.rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }*/



    __stop_stage1 = .;

    /DISCARD/ : { *(*) }
/*
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }*/

    /*.text   : { *(.sign) *(.text); *(.data); *(.rodata); *(.bss) }*/
    /*.data   : SUBALIGN(2) { *(.data); *(.rodata); }
    .bss    : { *(.bss); }*/
}