# @file asmCall_int12.S
# @author Diogo Gomes
# @brief Function to call int 12h from 16-bit real mode
# @version 0.1
# @date 2025-02-08
# 2025 Diogo Gomes

#include <bootloader/commonDefines.h>

.code16
.global asmCall_int12
.type asmCall_int12, @function

asmCall_int12:
    # Get our variables
    mov %sp, %di
    add $4, %di
    mov (%di), %ebx # Result memory location

    clc                 # Clear the carry flag
    xor %eax, %eax      # Clear %eax
    int $0x12           # Stores in %ax

    jc fail

    # No error, push %ax
    mov %eax, (%ebx)
    retl

fail:
    # %eax = 0 signals failure
    xor %eax, %eax
    mov %eax, (%ebx)
    retl