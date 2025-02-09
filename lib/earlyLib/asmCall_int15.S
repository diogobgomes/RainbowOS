# @file asmCall_int15.S
# @author Diogo Gomes
# @brief Function to call int 15h, %eax = 0xe820 from 16-bit real mode to get mmap
# @version 0.2
# @date 2025-02-08
# 2025 Diogo Gomes

#include <bootloader/commonDefines.h>

# Set constants
_int_15h_eax_const = 0xE820
_int_15h_edx_const = 0x534D4150

.code16
.global asmCall_int15
.type asmCall_int15, @function

asmCall_int15:
    # Get our variables
    mov %sp, %di
    add $4, %di
    mov (%di), %eax # Offset, we'll move to %di later
    add $4, %di
    mov (%di), %ecx # Segment
    mov %cx, %es
    add $4, %di
    mov (%di), %ebx # %ebx value
    add $4, %di
    mov (%di), %ecx # ebxPtr
    mov %ecx, (_temp)
    add $4, %di
    mov (%di), %ecx # eaxPtr
    mov %ecx, (_temp + 4)
    add $4, %di
    mov (%di), %ecx # ecxPtr
    mov %ecx, (_temp + 8)
    add $4, %di
    mov (%di), %ecx # carryPtr
    mov %ecx, (_temp + 12)

    mov %ax, %di

    # Set up call to int 15h
    mov $_int_15h_eax_const, %eax
    mov $_int_15h_edx_const, %edx
    mov $24, %ecx

    # Actually call int 15h    
    clc
    int $0x15

    # Test for carry
    jc fail


    # Move return values to memory (use %edx as pointer, as it's not needed)
    mov (_temp), %edx
    mov %ebx, (%edx)
    mov (_temp + 4), %edx
    mov %eax, (%edx)
    mov (_temp + 8), %edx
    mov %ecx, (%edx)
    retl


fail:
    # Don't even bother with the other ones, just set carry
    mov (_temp + 12), %edx
    movw $1, (%edx)
    retl


_temp:
    .double 0
    .double 0
    .double 0
    .double 0
