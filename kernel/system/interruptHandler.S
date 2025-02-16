# @file interruptHandler.S
# @author Diogo Gomes
# @brief Code to handle interrupts. It's just a macro that calls proper C++
# functions to handle whatever was needed
# @version 0.1
# @date 2025-02-11

.macro SAVE_CONTEXT
    pushal
.endm

.macro RESTORE_CONTEXT
    popal
.endm

.macro CREATE_HANDLER_NO_ERR number
.global _handler\number
.align 16
_handler\number:
    # Push the interrupt numnber
    xchgw %bx, %bx # Magic bochs for debugging
    # TODO see if it's necessary
    pushl $0 # Push dummy error code
    pushl $\number
    jmp _handler_stub_no_err
.endm

.macro CREATE_HANDLER_ERR number
.global _handler\number
.align 16
_handler\number:
    # Error code was pushed to the stack, just push number
    xchgw %bx, %bx # Magic bochs for debugging
    pushl $\number
    jmp _handler_stub_err
.endm

_handler_stub_err:
    SAVE_CONTEXT

    # Fetch interrupt number, error code
    # 16*8 = size pushed to stack = 384
    #mov 120(%rsp), %rdi # Int number
    #mov 128(%rsp), %rsi # Error code (136 = 128 + 8)

    call interruptHandler

    xchgw %bx, %bx

    RESTORE_CONTEXT

    add $8, %esp # Added by the _handler\number code

    iret

_handler_stub_no_err:
    SAVE_CONTEXT

    # Fetch interrupt number
    # 16*8 = size pushed to stack = 128
    #mov 120(%rsp), %rdi # Int number
    #mov $0, %rsi # Dummy error code


    call interruptHandler

    xchgw %bx, %bx

    RESTORE_CONTEXT

    add $8, %esp # Added by the _handler\number code

    iret

    
# Create all necessary interrupts
CREATE_HANDLER_NO_ERR   0 # Division Error
CREATE_HANDLER_NO_ERR   1 # Debug
CREATE_HANDLER_NO_ERR   2 # Non-maskable interrupt
CREATE_HANDLER_NO_ERR   3 # Breakpoint
CREATE_HANDLER_NO_ERR   4 # Overflow
CREATE_HANDLER_NO_ERR   5 # Bound range exceeded
CREATE_HANDLER_NO_ERR   6 # Invalid opcode
CREATE_HANDLER_NO_ERR   7 # Device not available
CREATE_HANDLER_ERR      8 # Double Fault
CREATE_HANDLER_NO_ERR   9 # Not used
CREATE_HANDLER_ERR      10 # Invalid TSS
CREATE_HANDLER_ERR      11 # Segment not present
CREATE_HANDLER_ERR      12 # Stack-segment fault
CREATE_HANDLER_ERR      13 # General protection fault
CREATE_HANDLER_ERR      14 # Page fault
CREATE_HANDLER_NO_ERR   15 # Reserved
CREATE_HANDLER_NO_ERR   16 # x87 Floating point exception
CREATE_HANDLER_ERR      17 # Alignement check
CREATE_HANDLER_NO_ERR   18 # Machine check
CREATE_HANDLER_NO_ERR   19 # SIMD Floating-Point exception
CREATE_HANDLER_NO_ERR   20 # Virtualization exception
CREATE_HANDLER_ERR      21 # Control protection exception
CREATE_HANDLER_NO_ERR   22 # Reserved
CREATE_HANDLER_NO_ERR   23
CREATE_HANDLER_NO_ERR   24
CREATE_HANDLER_NO_ERR   25
CREATE_HANDLER_NO_ERR   26
CREATE_HANDLER_NO_ERR   27
CREATE_HANDLER_NO_ERR   28 # Hypervisor Injection Exception
CREATE_HANDLER_ERR      29 # VMM comunication exception
CREATE_HANDLER_ERR      30 # Security Exception
CREATE_HANDLER_NO_ERR   31 # Reserved

#include "interruptAutoGen.inc"