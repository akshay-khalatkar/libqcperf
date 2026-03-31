# ============================================================================
# QcPerf - Qualcomm Performance Profiler Library
# Main CMakeLists.txt file
# ============================================================================

# Minimum CMake version required for modern CMake features
# Version 3.15+ provides improved target property handling and generator expressions
cmake_minimum_required(VERSION 3.15)

# ============================================================================
# Version information
# ============================================================================
# Enable version information generation
# This creates a header with version details, build timestamp, etc.
add_subdirectory(version_info)

# ============================================================================
# Project declaration
# ============================================================================
# - VERSION: Semantic versioning (major.minor.patch)
# - DESCRIPTION: Brief description of the project
# - LANGUAGES: Programming languages used (C only for this project)
project(QcPerf
    VERSION "${QCPERF_VERSION}"
    DESCRIPTION "Qualcomm Performance Profiler Library v${QCPERF_VERSION}"
    LANGUAGES C CXX
)


# ============================================================================
# Build configuration
# ============================================================================
# Include the combined build configuration from cmake/BuildConfig.cmake
# This centralizes platform detection, compiler flags, and other build settings
include(cmake/BuildConfig.cmake)


# ============================================================================
# Target Hardware Version
# ============================================================================
# Specify the hardware target version to select the appropriate thermal zone
# handling strategy at compile time:
#   V1  - Generic zone support (zones reported as raw BIOS names e.g. \_SB.TZxx)
#   V2  - Named zone support  (zones reported with descriptive names)
#   AUTO - Runtime detection: tries named match first, falls back to generic (default)
set(QCPERF_TARGET_VERSION "AUTO" CACHE STRING "Target hardware version (V1, V2, AUTO)")
set_property(CACHE QCPERF_TARGET_VERSION PROPERTY STRINGS V1 V2 AUTO)

if(QCPERF_TARGET_VERSION STREQUAL "V1")
    add_compile_definitions(QCPERF_TARGET_V1)
    message(STATUS "Target version: V1 (generic thermal zone support)")
elseif(QCPERF_TARGET_VERSION STREQUAL "V2")
    add_compile_definitions(QCPERF_TARGET_V2)
    message(STATUS "Target version: V2 (named thermal zone support)")
else()
    message(STATUS "Target version: AUTO (runtime detection, supports V1 and V2)")
endif()

# ============================================================================
# Component subdirectories
# ============================================================================
# Add each component in dependency order:
# - core: Main library functionality
# - backends: Performance monitoring backend implementations
# - utils: Utility libraries used by core and backends
# - core-test: Test application for the core library
add_subdirectory(core)
add_subdirectory(backends)
add_subdirectory(utils)
add_subdirectory(test_app)

# ============================================================================
# Configuration summary
# ============================================================================
# Print a summary of the build configuration for user verification
message(STATUS "")
message(STATUS "QcPerf Configuration Summary:")
message(STATUS "  Version: ${QCPERF_VERSION}")
message(STATUS "  Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER}")
message(STATUS "  CXX Compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "  System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "  Processor: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  Target version: ${QCPERF_TARGET_VERSION}")
message(STATUS "")
