# @file protectedModeCall.S
# @author Diogo Gomes
# @brief Function that allows for the calling of 16-bit real mode functions
# from 32-bit protected mode, and returns to it
# @version 0.2
# @date 2025-02-08
# 2025 Diogo Gomes

#include <bootloader/commonDefines.h>

.code32
.global realModeCall
.type realModeCall, @function

realModeCall:
    # First thing, save 32-bit execution environment
    # Stack pointers and return address
    # In total, need 12 bytes (two quads are enough)
    pop %eax
    mov %eax, (_temp)
    mov %ebp, (_temp + 4)
    mov %esp, (_temp + 8)

    # Start by setting up 16-bit protected mode
    cli
    cld
    ljmp $CODE16_SEGMENT, $.pm16Entry

.code16
.pm16Entry:
    # Reload data segments
    mov $DATA16_SEGMENT, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    # Disable PE bit in cr0
    mov %cr0, %eax
    and	~0x01, %al
    mov %eax, %cr0
    ljmp $0, $.realEntry

.realEntry:
    # Setup 16-bit stack. Since the stack is in the first segment anyway, no
    # need to worry about segment:offset, since segment = 0
    mov %sp, %bp
    mov $0, %ax # Need an intermediate value, can't load directly to segments
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    # Jump into the 16-bit function
    pop %eax
    call *%eax

    # Now start return to 32-bit pm mode (easy)
    cli
    cld

    mov %cr0, %eax
    or $1, %eax
    mov %eax, %cr0

    ljmp $CODE32_SEGMENT, $.pm32Entry

.code32
.pm32Entry:
    # Reload the data segments
    mov $DATA32_SEGMENT, %eax
    mov %eax, %ds
    mov %eax, %es
    mov %eax, %fs
    mov %eax, %gs
    mov %eax, %ss

    # Restore the 32-bit execution environment
    mov (_temp + 8), %esp
    mov (_temp + 4), %ebp
    mov (_temp), %eax
    push %eax

    ret






_temp:
    .quad 0
    .quad 0