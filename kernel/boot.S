# @file boot.S
# @author Diogo Gomes
# @brief Small assembly stub, whose purpose is to setup stack, and call kloader
# @version 0.1
# @date 2025-02-08

# Multiboot header constants
.set ALIGN, 1<<0 # Align loaded modules on page boundaries
.set MEMINFO, 1<<1 # Ask for memory map
.set FLAGS, ALIGN | MEMINFO # multiboot_header->flags
.set MULTIBOOT_MAGIC, 0x1BADB002 # multiboot_header->magic
.set CHECKSUM, -(MULTIBOOT_MAGIC + FLAGS) # multiboot_header->checksum

# Declare the multiboot header, make sure it's near the
# beggining of the binary
.section .multiboot_header
.align 4
.long MULTIBOOT_MAGIC
.long FLAGS
.long CHECKSUM

# Stuff for the GDT
#define CODE32_KSEGMENT              0x08
#define DATA32_KSEGMENT              0x10
#define CODE32_USEGMENT              0x18
#define DATA32_USEGMENT              0x20


# BSS
# Stack, and paging tables
.section .bss
.align 4096
#p4_table:
#.skip 4096
#p3_table:
#.skip 8192 # 4096 * 2, we'll need two p3 tables
#p2_table:
#.skip 8192 # 4096 * 2, we'll use two p2 tables
stack_bottom:
.skip 16384 # 16 KiB
stack_top:


.code32
.section .text
.extern kmain
.global _start
.type _start, @function

# Init entry point
_start:
    cli # Just in case they were on

    # Setup stack
    mov $stack_top, %esp
    mov $stack_top, %ebp

    # Even though there is a GDT set up by our bootloader (and GRUB also does
    # it, let's be nice people and set one up (it's not hard and barely adds
    # any size to the binary)
    push %eax # Preserve %eax

    cli
    mov $gdt_desc, %eax
    lgdt (%eax)
    mov %cr0, %eax
    or $1, %eax
    mov %eax, %cr0

.gdt_reload:
    # Reload data segments
    mov $DATA32_KSEGMENT, %eax
    mov %eax, %ds
    mov %eax, %es
    mov %eax, %fs
    mov %eax, %gs
    mov %eax, %ss

    pop %eax # Restore %eax

    # Push variables to the stack
    push %edx # Terminal index
    push %ebx # Multiboot info
    push %eax # Multiboot flag, for checking

    # Push also beggining of paging, to be used
    #mov $p4_table, %eax
    #push %eax

    call kmain

    # Should never get here
    cli
1:  hlt
    jmp 1b

.data

# Since the GDT is fixed, let's just put it in and forget about it
gdt_start:
gdt_null_segment:
    .quad 0
gdt_32_kcode_segment:
    .quad 0x00CF9A000000FFFF
gdt_32_kdata_segment:
    .quad 0x00CF92000000FFFF
gdt_32_ucode_segment:
    .quad 0x00CFFA000000FFFF
gdt_32_udata_segment:
    .quad 0x008FF2000000FFFF
# TODO at some point, we must have a Task State Segment here
gdt_end:

.equ GDT_START, gdt_start

gdt_desc:
    .word gdt_end - gdt_start - 1
    .long GDT_START
