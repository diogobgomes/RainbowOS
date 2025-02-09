message(STATUS "Trying to find qemu...")

find_program(
    qemu
    PATHS /usr/bin/
    NAMES qemu-system-i386
)

if(qemu)
    set(qemu_FOUND 1)
    set(qemu_EXECUTABLE qemu-system-i386)
    message(STATUS "Found qemu with path: ${qemu_EXECUTABLE}")
endif()
if(qemu-NOTFOUND)
    message(STATUS "qemu not found, not enabling that target")
endif()