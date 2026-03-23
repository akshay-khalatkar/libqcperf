# qcv library CMake configuration
# Builds platform-specific condition variable implementation based on target OS

cmake_minimum_required(VERSION 3.13.5)

if(QCPERF_OS_WINDOWS)
    add_library(QcPerfQCv STATIC
        src/windows/qcv.c
    )
else()
    add_library(QcPerfQCv STATIC
        src/linux/qcv.c
    )
endif()

# Include public headers
target_include_directories(QcPerfQCv PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
