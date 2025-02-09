# @file asmCall_int13.S
# @author Diogo Gomes
# @brief Function to call int 13h from 16-bit real mode
# @version 0.1
# @date 2025-02-08
# 2025 Diogo Gomes

#include <bootloader/commonDefines.h>

.code16
.global asmCall_int13
.type asmCall_int13, @function

asmCall_int13:
    # Get our variables
    mov %sp, %di
    add $4, %di
    mov (%di), %edx # Disk
    add $4, %di
    mov (%di), %ecx # Result ptr

    # Call int 13h
    mov $_dap_memory_location, %si
    mov $0x4200, %eax
    int $0x13

    jc fail

    # No error, return 0 in (%ecx)
    movb $0, (%ecx)
    retl

fail:
    movb %ah, (%ecx)
    retl