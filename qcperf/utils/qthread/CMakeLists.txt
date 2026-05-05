# qthread library CMake configuration
# Builds platform-specific thread implementation based on target OS

if(QCPERF_OS_WINDOWS)
    add_library(QcPerfQThread STATIC
        src/windows/QThread.c
    )
else()
    add_library(QcPerfQThread STATIC
        src/linux/QThread.c
    )
endif()

# Link with OS-specific thread libraries
target_link_libraries(QcPerfQThread PRIVATE
    ${_OS_THREAD}
)

# Include public headers
target_include_directories(QcPerfQThread PUBLIC
    inc
)
