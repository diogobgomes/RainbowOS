# @file checkCPUID.S
# @author Diogo Gomes
# @brief Function to check presence of CPUID instruction
# @version 0.1
# @date 2025-02-13
# 2025 Diogo Gomes

.code32
.global check_CPUID_available
.type check_CPUID_available, @function
check_CPUID_available:
    # Checks if CPUID is supported by trying to flip bit 21 in
    # the FLAGS register. If we can, CPUID is available

    # Copy FLAGS to %eax
    pushfl
    pop %eax
    mov %eax, %ecx

    # Flip bit 21
    xor $1 << 21, %eax

    # Copy %eax to flags
    push %eax
    popfl

    # Copy back to %eax
    pushfl
    pop %eax

    # Restore FLAGS with old version in %ecx
    push %ecx
    popfl

    # Compare
    cmp %eax, %ecx
    je .no_CPUID

    # Return true
    mov $1, %eax
    ret

.no_CPUID: # Return false
    mov $0, %eax
    ret


