# BOOTLOADER CMAKE - v0.1
#
# Bootloader CMakeLists.txt, to be included by the top level CMakeLists.txt
#
#
# 2025 Diogo Gomes

# Export compilation database
set( CMAKE_EXPORT_COMPILE_COMMANDS on )

# Preprocessor directives
add_compile_definitions(__bootloader__)

#include(${CMAKE_SOURCE_DIR}/include/sources.cmake)

add_executable(
    stage0.bin
    stage0.S
)

target_link_options(
    stage0.bin PRIVATE
    -T ${CMAKE_CURRENT_SOURCE_DIR}/stage0.ld
)

# Check size
add_custom_command(TARGET stage0.bin POST_BUILD
    # Get file size
    COMMAND /bin/bash -c "if [ \$(ls -l stage0.bin | awk '{print \$5}') -gt 440 ]; then \
                            echo \"It's way too big!\"; \
                            rm stage0.bin; \
                            false; \
                            fi"
    
    COMMENT "Verifying stage0.bin size"
    VERBATIM
)

add_executable(
    stage1.bin
    stage1.S
    stage1main.cpp
    ${HEADER_FILES}
)

#target_link_options(
#    stage1.bin PRIVATE
#    -T ${CMAKE_CURRENT_SOURCE_DIR}/stage1.ld
#    -nostdlib
#    -lgcc
#    -nostartfiles
#)

# Set link options manually because cmake isn't cooperating
set( STAGE1_LINK_OPTIONS
    "-T ${CMAKE_CURRENT_SOURCE_DIR}/stage1.ld -nostdlib -nostartfiles"
)

target_link_libraries(
    stage1.bin PUBLIC lib.ar
)

# Make sure -lgcc is at the end of the link command (cause it needs to be, ugh)
set( CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_COMPILER} <CMAKE_CXX_LINK_FLAGS> <FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES> ${STAGE1_LINK_OPTIONS} -lgcc" )
set( CMAKE_C_LINK_EXECUTABLE "${CMAKE_C_COMPILER} <CMAKE_C_LINK_FLAGS> <FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES> ${STAGE1_LINK_OPTIONS} -lgcc" )


# Check size, inject necessary things into binary
add_custom_command(TARGET stage1.bin POST_BUILD
    # Get file size (65024 bytes is available space)
    COMMAND /bin/bash -c "if [ \$(ls -l stage1.bin | awk '{print \$5}') -gt 65024 ]; then \
                            echo \"It's way too big!\"; \
                            rm stage1.bin; \
                            false; \
                            fi"
    
    COMMAND ${CMAKE_COMMAND} -E echo "Injecting size into binary"

    COMMAND /bin/bash -c "size=\$(ls -l stage1.bin | awk 'function ceil(x,y){y=int(x); return(x>y?y+1:y)} {print ceil(\$5/512)}'); \
                            printf \"%04x\" \"\$size\" | xxd -r -p | dd of=stage1.bin bs=1 count=2 seek=2 conv=notrunc,sparse,swab"

    COMMENT "Verifying stage1.bin size"
    VERBATIM
)