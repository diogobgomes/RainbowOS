# List of header files, to be included in CMakeLists
#
#
# 2025 Diogo Gomes

set(
    HEADER_FILES
    bootloader/commonDefines.h
    devices/BIOSVideoIO.hpp
    earlyLib/diskRead16.hpp
    earlyLib/memory.hpp
    earlyLib/memoryDetection16.hpp
    fs/fat32.hpp
    fs/mbr.hpp
    klib/cstdlib.hpp
    klib/io.hpp
    klib/stdlib.h
    klib/string.h
    sys/elf.h
    sys/multiboot.h
    kernel/interrupts.hpp
    kernel/gdt.h
    kernel/cpu.hpp
)

list(TRANSFORM HEADER_FILES PREPEND ${CMAKE_SOURCE_DIR}/include/)
