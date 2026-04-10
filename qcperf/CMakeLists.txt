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

# Enable testing
enable_testing()

# ============================================================================
# Component subdirectories
# ============================================================================
# Add each component in dependency order:
# - third-party: Third-party libraries (Unity testing framework)
# - core: Main library functionality
# - backends: Performance monitoring backend implementations
# - utils: Utility libraries used by core and backends
# - test-app: Test application for the core library
add_subdirectory(third-party)
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
message(STATUS "")
