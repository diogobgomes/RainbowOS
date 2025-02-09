# DOCS CMAKE - v0.1
#
# Docs CMakeLists.txt, to be included by the top level CMakeLists.txt
# Generates the Doxygen file, and adds target to build docs
#
#
# 2023 Diogo Gomes

message(STATUS "Generating Doxyfile from Doxyfile.in...")
configure_file("${CMAKE_SOURCE_DIR}/docs/Doxyfile.in" "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" @ONLY)

add_custom_target(
    docs
    COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    COMMENT "Generating docs..."
    VERBATIM
)