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

include(${CMAKE_SOURCE_DIR}/include/sources.cmake)

add_executable(
    kernel.bin
    boot.S
    kmain.cpp
    ${HEADER_FILES}
)

target_link_options(
    kernel.bin PRIVATE
    -T ${CMAKE_CURRENT_SOURCE_DIR}/kernel.ld
    -nostdlib
    -lgcc
    -nostartfiles
)

target_link_libraries(
    kernel.bin PUBLIC lib.ar
)
