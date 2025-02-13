# LIB CMAKE - v0.1
#
# Lib CMakeLists.txt, to be included by the top level CMakeLists.txt
#
#
# 2025 Diogo Gomes

# Export compilation database
set( CMAKE_EXPORT_COMPILE_COMMANDS on )

# Preprocessor directives
add_compile_definitions(__lib__)

#include(${CMAKE_SOURCE_DIR}/include/sources.cmake)

# VARIABLES
file(GLOB_RECURSE LIB_SRC_LIST CONFIGURE_DEPENDS "*.cpp" "*.c" "*.S")


add_library(
    lib.ar STATIC
    ${LIB_SRC_LIST}
    ${HEADER_FILES}
)

target_include_directories(lib.ar PUBLIC ${CMAKE_SOURCE_DIR}/include)