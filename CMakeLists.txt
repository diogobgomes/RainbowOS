# TOP LEVEL CMAKE - v0.1
#
# Top level CMakeLists.txt
#
#
# 2025 Diogo Gomes

cmake_minimum_required(VERSION 3.28)

# Force out-of-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds are not allowed. Please use an out-of-source build.")
endif()

# Set default build type to DEBUG
set(default_build_type "Debug")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  # FIXME add tracemax here
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Set up toolchain
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/config/toolchain.cmake" CACHE STRING "Toolchain file for cmake")

# Configure version, languages
set(RainbowOS_VERSION 0.0.0)
set(RainbowOS_LANGUAGES ASM CXX C )

project(RainbowOS VERSION ${RainbowOS_VERSION} LANGUAGES ${RainbowOS_LANGUAGES})

# Header files
include_directories("${CMAKE_SOURCE_DIR}/include")

# Setup compiler
set(
    C_COMPILER_OPTIONS
    "-Wall -Wextra -pedantic -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wno-long-long -Wuninitialized -Wconversion -Wstrict-prototypes"
)
set(
    CXX_COMPILER_OPTIONS
    "-pedantic -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Winline"
)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${C_COMPILER_OPTIONS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXX_COMPILER_OPTIONS}")

# Set variables
set(DISKIMAGE "diskimage.dd" CACHE STRING "Name of the diskimage to be generated")
set(LOOPBACK "/dev/loop0" CACHE STRING "Loopback device to be used")
set(SCRIPTS_DIR "${CMAKE_SOURCE_DIR}/build-scripts")

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/config)

# Subdirectories
add_subdirectory(lib)
add_subdirectory(bootloader)
add_subdirectory(kernel)

# Add gdb files
message(STATUS "Creating gdb files...")
execute_process( COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/config/gdbinit ${CMAKE_BINARY_DIR}/.gdbinit)
execute_process( COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_BINARY_DIR}/.gdb_history)


# Diskimage generation
add_custom_target(
    generate-diskimage
    COMMAND ${SCRIPTS_DIR}/setup-diskimage.sh ${DISKIMAGE} ${CMAKE_SOURCE_DIR} ${LOOPBACK}
    COMMENT "Checking presence of diskimage..."
    VERBATIM
)

set(MOUNT_DIR "${CMAKE_CURRENT_BINARY_DIR}/mount_dir"
                        CACHE STRING "Full path to mountpoint")

set(STAGE0_BIN "${CMAKE_CURRENT_BINARY_DIR}/bootloader/stage0.bin")
set(STAGE1_BIN "${CMAKE_CURRENT_BINARY_DIR}/bootloader/stage1.bin")
set(KERNEL_BIN "${CMAKE_CURRENT_BINARY_DIR}/kernel/kernel.bin")
    
add_custom_target(
    diskimage
    COMMAND ${SCRIPTS_DIR}/copy-diskimage.sh ${DISKIMAGE} ${LOOPBACK} ${CMAKE_SOURCE_DIR} ${MOUNT_DIR} ${STAGE0_BIN} ${STAGE1_BIN} ${KERNEL_BIN}
    DEPENDS bootloader/stage0.bin bootloader/stage1.bin kernel/kernel.bin generate-diskimage
    COMMENT "Final diskimage generation..."
    VERBATIM
)

add_custom_target(
    mountdir
    COMMAND sudo -A ${SCRIPTS_DIR}/mountdirs.sh ${MOUNT_DIR} ${LOOPBACK} ${DISKIMAGE} `whoami`
    COMMENT "Attempting to mount loopback..."
)

add_custom_target(
    umountdir
    COMMAND sudo -A umount ${MOUNT_DIR} && sudo -A losetup -d ${LOOPBACK}
    COMMENT "Attempting to unmount loopback..."
    VERBATIM
)

find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_subdirectory(docs)
else()
    message(STATUS "Doxygen not found, not building docs")
endif()

find_package(qemu)
if(qemu_FOUND)
    add_custom_target(
        qemu
        COMMAND ${qemu_EXECUTABLE} -drive file=${DISKIMAGE},format=raw
        COMMENT "Launching qemu..."
        DEPENDS diskimage
        VERBATIM
    )
endif()