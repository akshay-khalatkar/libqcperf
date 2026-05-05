# QTime library CMake configuration
# Builds platform-specific time utility implementation based on target OS

if(QCPERF_OS_WINDOWS)
    add_library(QcPerfQTime STATIC
        src/windows/qtime.c
    )
else()
    add_library(QcPerfQTime STATIC
        src/linux/qtime.c
    )
endif()

# Link with OS-specific time libraries
target_link_libraries(QcPerfQTime PRIVATE
)

# Include public headers
target_include_directories(QcPerfQTime PUBLIC
    inc
)
