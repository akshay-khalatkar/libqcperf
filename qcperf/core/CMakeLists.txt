# ============================================================================
# QcPerf Core Library
# ============================================================================
# This CMakeLists.txt builds the core library of the QcPerf project, which
# provides the main API for performance monitoring and profiling.

# ============================================================================
# Library Target
# ============================================================================
# Create the main QcPerfCore library as a static library
# This library implements the core functionality of the QcPerf API
add_library(QcPerfCore STATIC
    src/qcperf.c
)

# ============================================================================
# Target Properties
# ============================================================================
# Set output name and position independent code settings
# - OUTPUT_NAME: Sets the actual filename of the library
# - POSITION_INDEPENDENT_CODE: Disabled as this is a static library
set_target_properties(QcPerfCore PROPERTIES
    OUTPUT_NAME "qcperfCore"
    POSITION_INDEPENDENT_CODE OFF
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
