# @file stage1.S
# @author Diogo Gomes
# @brief Bootloader assembly stub, enables protected mode, and passes control
# over to the main C++ function _stage1Main. Also, jumps to init
# @version 0.1
# @date 2025-02-08
# 2025 Diogo Gomes

#include <bootloader/commonDefines.h>

.code16
.extern __stop_stage1
.extern _stage1Main


# Stage1 signature
.section .header
.word _stage1_magic
.word 0 # Number of sectors, will be replaced by build script

.section .text
.global _start
.type _start, @function
_start:
    # Stack is already set up
    # Segments are already set up
    # All we need is to jump to C++

    # First of all, save %dl
    mov %dx, (_temp)

# Now, start by enabling A20 line
.a20_stuff:
    call check_a20
    mov $1, %bx
    cmp %ax, %bx
    je a20_gate_enabled

    # Line is disabled
    leaw a20_line_disabled_msg, %si
    call write_loop

    call enable_20_gate
    jmp a20_done

a20_gate_enabled:
    leaw a20_line_skip_msg, %si
    call write_loop

a20_done:
    # Get cursor position and everything
    # Int 10h, %ah = 0x03, %bh = 0 (page)
    mov $0x03, %ah
    mov $0, %bh
    int $0x10

    # Now we have, for what matters, %dh = row, %dl = columns

    mov %dx, (_temp + 2)

    # Now do GDT stuff
    cli

    mov $gdt_desc, %eax
    lgdt (%eax)

    mov %cr0, %eax
    or $1, %eax
    mov %eax, %cr0

    ljmp $CODE32_SEGMENT, $.pmEntry

.code32
.pmEntry:
    # Reload data segments
    mov $DATA32_SEGMENT, %eax
    mov %eax, %ds
    mov %eax, %es
    mov %eax, %fs
    mov %eax, %gs
    mov %eax, %ss

    # Pass cursor position
    xor %ax, %ax
    mov (_temp + 2), %ax
    push %eax

    # Push heap location to stack
    mov $__stop_stage1, %eax
    push %eax

    # Recover %dl
    xor %edx, %edx
    mov (_temp), %dx
    push %edx

    # Call main
    call _stage1Main

    cli
1:  hlt # Should never get here, actually
    jmp 1b

.global jumpKernel
.type jumpKernel, @function

jumpKernel:
    # Get arguments
    mov %sp, %di
    add $4, %di
    mov (%di), %ecx # Address
    add $4, %di
    mov (%di), %ebx # mbInfo pointer
    add $4, %di
    mov (%di), %edx # Terminal index

    # Prepare multiboot
    mov $0x2BADB003, %eax # FIXME Change this to our own thing

    # We're frankly ready to jump, so let's go
    jmp *%ecx

################################################################################
#
# FUNCTIONS
#
################################################################################
.code16
check_a20:
    # Function to check whether A20 is enabled
    # Returns:
    # %ax == 0 if A20 is disabled
    # %ax == 1 if A20 is enabled

    # Preserve state of registers
    pushf
    push %ds
    push %es
    push %di
    push %si
    cli

    # Where we are testing
    # 0x0000:0x0500(0x00000500) -> %ds:%si
    # 0xffff:0x0510(0x00100500) -> %es:%di
    xor %ax,%ax # %ax = 0
    mov %ax, %es

    not %ax # %ax = 0xffff
    mov %ax, %ds

    mov $0x0500, %di
    mov $0x0510, %si

    # Preserve what's at the locations
    mov %es:(%di), %al
    push %ax

    mov %ds:(%si), %al
    push %ax

    # Write stuff to these locations
    movw $0x00, %es:(%di)
    movw $0xff, %ds:(%si)
    
    # Compare
    cmpw $0xff, %es:(%di) # If equal, we've wrapped around

    # Restore locations
    pop %ax
    mov %al, %ds:(%si)
    pop %ax
    mov %al, %es:(%di)

    mov $0, %ax
    je .check_a20_exit

    mov $1, %ax # They're not equal, A20 is enabled

    .check_a20_exit:
    pop %si
    pop %di
    pop %es
    pop %ds
    popf

    ret

enable_20_gate:
    # We're gonna try, in order, int 0x15 enable, keyboard controller, and fast a20
    .int15_test:
        clc
        mov $0x2403, %ax
        int $0x15
        jc .keyboard_enable # Int 15h is not supported
        test %ah,%ah
        jnz .keyboard_enable # Int 15h is not supported

    .int15_enable:
        mov $0x2401, %ax
        int $0x15
        jc .keyboard_enable
        test %ah,%ah
        jnz .keyboard_enable

        # Test it
        call check_a20
        mov $1, %bx
        cmp %ax, %bx
        je .a20_gate_enabled

    .keyboard_enable:
        leaw a20_line_attempt_keyboard_msg, %si
        call write_loop

        cli

        # Disablle keyboard
        call .a20_wait
        mov $0xad, %al
        out %al, $0x64

        # Read from input
        call .a20_wait
        mov $0xd0, %al
        out %al, $0x64

        call .a20_wait_2
        in $0x60, %al
        push %eax

        # Write to output
        call .a20_wait
        mov $0xd1, %al
        out %al, $0x64

        call .a20_wait
        pop %eax
        or $2,%al
        out %al, $0x60

        call .a20_wait
        mov $0xae, %al
        out %al, $0x64

        call .a20_wait
        sti

        # Test it
        call check_a20
        mov $1, %bx
        cmp %ax, %bx
        je .a20_gate_enabled

    .fast_a20_enable:
        leaw a20_line_attempt_fast_msg, %si
        call write_loop

        in $0x92, %al
        test $2, %al
        jnz .a20_gate_failed

        or $2, %al
        and $0xfe, %al
        out %al, $0x92

        # Test it
        call check_a20
        mov $1, %bx
        cmp %ax, %bx
        je .a20_gate_enabled

.a20_gate_failed:
    # We've exausted the options to enable the A20 gate, so we're giving up
    leaw a20_line_error_msg, %si
    jmp error

.a20_wait:
    in $0x64, %al
    test $2, %al
    jnz .a20_wait
    ret

.a20_wait_2:
    in $0x64, %al
    test $1, %al
    jz .a20_wait_2
    ret

.a20_gate_enabled:
    leaw a20_line_enabled_msg, %si
    call write_loop
    ret

write_loop:
    xor %bh, %bh #Set page 0 for int 10h
    lodsb #Load *(%si) to %al (and increment)
    orb %al, %al #Zero is end of string
    jz write_done
    mov $0x0e, %ah #Teletype mode
    int $0x10
    jmp write_loop

write_done:
    ret

error:
    call write_loop

    # Halt
    cli
    hlt

################################################################################
#
# MESSAGES
#
################################################################################

a20_line_disabled_msg:
    .asciz "A20 line is not enabled, attempting to enable it...\n\r"

a20_line_enabled_msg:
    .asciz "A20 line enabled successfully!\n\r"

a20_line_error_msg:
    .asciz "Error: Could not enable A20 line, aborting!"

a20_line_attempt_keyboard_msg:
    .asciz "Int 15h enable failed, attempting keyboard enable...\n\r"

a20_line_attempt_fast_msg:
    .asciz "Keyboard enable failed, attempting fast A20 enable... \n\r"

a20_line_skip_msg:
    .asciz "A20 line is already enabled, skipping enabling\n\r"


.data
_temp:
    .double 0

gdt_start:
gdt_null_segment:
    .quad 0
gdt_32_code_segment:
    .quad 0x00CF9A000000FFFF
gdt_32_data_segment:
    .quad 0x00CF92000000FFFF
gdt_16_code_segment:
    .quad 0x008F9A000000FFFF
gdt_16_data_segment:
    .quad 0x008F92000000FFFF
gdt_end:

.equ GDT_START, gdt_start

gdt_desc:
    .word gdt_end - gdt_start - 1
    .long GDT_START
