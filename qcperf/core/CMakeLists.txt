# ============================================================================
# QcPerf Core Library
# ============================================================================
# This CMakeLists.txt builds the core library of the QcPerf project, which
# provides the main API for performance monitoring and profiling.

# ============================================================================
# Library Target
# ============================================================================
# Build as SHARED when BUILD_SHARED=ON is passed, otherwise STATIC (default).
# Usage:
#   cmake -DBUILD_SHARED=ON  ...   -> builds libqcperfCore.so / qcperfCore.dll
#   cmake                    ...   -> builds libqcperfCore.a  / qcperfCore.lib
if(BUILD_SHARED)
    set(QCPERF_CORE_LIB_TYPE SHARED)
else()
    set(QCPERF_CORE_LIB_TYPE STATIC)
endif()

add_library(QcPerfCore ${QCPERF_CORE_LIB_TYPE}
    src/qcperf.c
)

# ============================================================================
# Target Properties
# ============================================================================
# Set output name and position independent code settings
# - OUTPUT_NAME: Sets the actual filename of the library
set_target_properties(QcPerfCore PROPERTIES
    OUTPUT_NAME "qcperfCore"
    POSITION_INDEPENDENT_CODE ${BUILD_SHARED}
)

# ============================================================================
# Include Directories
# ============================================================================
# Configure include paths for the core library:
# - inc/: Public API headers for the core library
# - include/: Generated version information header
# - backends/inc: Backend interface headers
target_include_directories(QcPerfCore
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>  # For generated versionInfo.h
        ${CMAKE_SOURCE_DIR}/backends/inc
)

# ============================================================================
# Compile Definitions
# ============================================================================
# Set the correct export/import macro based on the library type:
if(BUILD_SHARED)
    target_compile_definitions(QcPerfCore PRIVATE
        QCPERF_SHARED_LIBRARY
    )
    if(NOT MSVC)
        target_compile_options(QcPerfCore PRIVATE -fvisibility=hidden)
    endif()
else()
    target_compile_definitions(QcPerfCore PUBLIC
        QCPERF_STATIC_LIBRARY
    )
endif()

# ============================================================================
# Dependencies
# ============================================================================
# Link against required libraries:
# - QcPerfBackends: Unified backend interface that includes all backends
target_link_libraries(QcPerfCore
    PUBLIC
        QcPerfBackends
)

# ============================================================================
# Compiler Options
# ============================================================================
# Set compiler-specific options for the core library
# - MSVC (Visual Studio): Use warning level 4, don't treat warnings as errors
# - GCC/Clang: Enable all warnings, extra warnings, and pedantic mode
if(MSVC)
    target_compile_options(QcPerfCore PRIVATE
        /W4
        /WX-  # Don't treat warnings as errors (can be changed to /WX if needed)
    )
else()
    target_compile_options(QcPerfCore PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()
