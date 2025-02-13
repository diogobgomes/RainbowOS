# KERNEL CMAKE - v0.1
#
# Kernel CMakeLists.txt, to be included by the top level CMakeLists.txt
#
#
# 2025 Diogo Gomes

# Export compilation database
set( CMAKE_EXPORT_COMPILE_COMMANDS on )

# Preprocessor directives
add_compile_definitions(__kernel__)

#include(${CMAKE_SOURCE_DIR}/include/sources.cmake)

add_executable(
    kernel.bin
    boot.S
    kmain.cpp
    ${HEADER_FILES}
)

#target_link_options(
#    kernel.bin PRIVATE
#    -T ${CMAKE_CURRENT_SOURCE_DIR}/kernel.ld
#    -nostdlib
#    -nostartfiles
#)

target_link_libraries(
    kernel.bin PUBLIC lib.ar
)

# Set link options manually because cmake isn't cooperating
set( KERNEL_LINK_OPTIONS
    "-T ${CMAKE_CURRENT_SOURCE_DIR}/kernel.ld -nostdlib -nostartfiles"
)

# Make sure -lgcc is at the end of the link command (cause it needs to be, ugh)
set( CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_COMPILER} <CMAKE_CXX_LINK_FLAGS> <FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES> ${KERNEL_LINK_OPTIONS} -lgcc" )
set( CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_COMPILER} <CMAKE_C_LINK_FLAGS> <FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES> ${KERNEL_LINK_OPTIONS} -lgcc" )
