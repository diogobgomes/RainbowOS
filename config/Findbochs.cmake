message(STATUS "Trying to find bochs...")

find_program(
    bochs
    NO_DEFAULT_PATH
    PATHS $ENV{HOME}/opt/bochs/bin/
    NAMES bochs
)

if(bochs)
    set(bochs_FOUND 1)
    message(STATUS "Found bochs with path: ${bochs}")
endif()
if(${bochs} STREQUAL bochs-NOTFOUND)
    message(STATUS "bochs not found, disabling targets")
endif()