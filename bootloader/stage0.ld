/* Stage0 linker script - v0.1

    
    2025 Diogo Gomes */

ENTRY(_start)
OUTPUT_FORMAT("binary")
OUTPUT_ARCH("i386")
phys = 0x0800;

SECTIONS
{
    . = phys;

    .text : {
        /**(.sign)*/
        /*stage0.o(.text);
        . = ALIGN(4);*/
        *(.text)
    }
}