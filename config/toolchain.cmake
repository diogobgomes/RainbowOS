# Default CMake toolchain file - v0.1
#
# Modify for use on own system
#
#
# 2023 Diogo Gomes

# Target OS
set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_SYSTEM_PROCESSOR i386)

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Tools
set(TOOLS $ENV{HOME}/opt/i686-elf-cross)
set(CMAKE_C_COMPILER ${TOOLS}/bin/i686-elf-gcc)
set(CMAKE_CXX_COMPILER ${TOOLS}/bin/i686-elf-g++)
set(CMAKE_ASM_COMPILER ${TOOLS}/bin/i686-elf-gcc)
set(CMAKE_LINKER ${TOOLS}/bin/i686-elf-ld)

# Required flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -ffreestanding")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG}  -ffreestanding")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE}  -ffreestanding")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -ffreestanding -fno-rtti -fno-exceptions")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}  -ffreestanding -fno-rtti -fno-exceptions")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}  -ffreestanding -fno-rtti -fno-exceptions")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -nostdlib -nostartfiles")