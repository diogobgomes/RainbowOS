# @file stage0.S
# @author Diogo Gomes
# @brief Small assembly stub that sits in the MBR, and whose purpose it is
# to set up some basic things, load stage1, and jump to it
# @version 0.1
# @date 2023-09-01

#include <bootloader/commonDefines.h>

.code16
.section .text
.global _start
.type _start, @function
_start:
    cli # Disable interrupts

    # We want to load the correct value ($0) on all segment registers
    mov $0, %ax # Need an intermediate value, can't load directly to segments
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss

    # We want now to copy ourselves to as low memory as possible
    mov $0x0100, %cx # 256 words in MBR
    mov $_initial_mbr_address, %si # Current MBR address
    mov $_final_mbr_address, %di # New MBR address: want to cut it finely
    rep movsw # Copy the MBR to the new address

    ljmp $0, $low_memory # Long jump to next line, setting %CS to 0

low_memory:
    # We must now set the stack pointer to a valid thing (but don't care about it)
    mov $_stack_pointer, %ax
    mov %ax, %sp
    mov %ax, %bp

    # Re-enable interrupts
    sti

    # Get everything set up for int 13h
    # First thing's first, reset the disk controller, test for error
    mov $0, %ah
    int $0x13
    # Test for error: jump if carry
    leaw reset_error_msg, %si
    jc error


    # Now we must read the second sector on the HDD, and look at the beggining
    # for the presence of stage1

    mov $_dap_memory_location, %di
    mov $_heap_location, %ax
    mov %ax, %fs
    mov $1, %cx
    mov $1, %ebx # Second sector
    call lba_read

    #jmp test_location

    # Check for stage1 signature
    mov $_heap_location, %bx
    movw (%bx), %ax
    mov $_stage1_magic, %bx
    leaw stage1_magic_notFound_error_msg, %si
    cmp %ax, %bx
    jne error
    
    # Read number of sectors to read for stage1
    mov $(_heap_location + 2), %bx
    movw (%bx), %cx # Sector count
    
    # READ
    # Only need to set %ebx, the rest is already done
    mov $1, %ebx # Second sector
    call lba_read

    # Reset %fs
    xor %ax, %ax
    mov %ax, %fs

    # Now, just jump to it!
    mov $(_heap_location + 4), %ax
    #xchgw %bx, %bx
    jmp *%ax

    # Should never get here
    cli
    hlt


lba_read:
    # Inputs:
    # %ebx - 32-bit LBA address
    # %cx - sector count
    # %es:%fs - buffer
    # %dl - drive number
    # %di - DAP location

    # Set up the DAP
    movb $0x10, (%di) # Size of DAP
    add $1, %di
    movb $0, (%di) # Unused, should be 0
    add $1, %di
    movw %cx, (%di) # Sector count
    add $2, %di
    movw %fs, (%di)
    add $2, %di
    movw %es, (%di)
    add $2, %di
    mov %ebx, (%di) # LBA

    sub $8, %di
    mov %di , %si

    #xchgw %bx,%bx

    mov $0x42, %ah
    int $0x13

    # Test for error
    leaw read_error_msg, %si
    jc error

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


msg:
    .asciz "dbgMsg"

reset_error_msg:
    .asciz "Error: Reset controller failed"

stage1_magic_notFound_error_msg:
    .asciz "Error: Stage1 magic not found"

read_error_msg:
    .asciz "Error: Read"

#Boot sector is 512 bytes long, 510 + 2 bytes for the signature
#.org 510
#.org 510
#.word 0xaa55
