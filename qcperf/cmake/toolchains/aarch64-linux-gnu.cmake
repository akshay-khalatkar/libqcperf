# ============================================================================
# QcPerf - AArch64 Linux Cross-Compilation Toolchain
# ============================================================================
# This toolchain file configures CMake to cross-compile for Linux AArch64
# using the ARM GNU Toolchain (aarch64-none-linux-gnu).
#
# Usage:
# set the environment variable before invoking cmake:
#   export AARCH64_TOOLCHAIN_PATH=/path/to/arm-gnu-toolchain
#   cmake -S qcperf -B build-aarch64 \
#       --toolchain qcperf/cmake/toolchains/aarch64-linux-gnu.cmake \
#       -DCMAKE_BUILD_TYPE=Release
#   cmake --build build-aarch64
# ============================================================================

# ----------------------------------------------------------------------------
# Target system identification
# ----------------------------------------------------------------------------
# Tell CMake we are building for Linux on AArch64 (not the host x86_64)
set(CMAKE_SYSTEM_NAME      Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# ----------------------------------------------------------------------------
# Forward AARCH64_TOOLCHAIN_PATH to try_compile sub-projects
# ----------------------------------------------------------------------------
# CMake re-evaluates the toolchain file during internal compiler ABI detection
# (try_compile). Without this, AARCH64_TOOLCHAIN_PATH would be undefined in
# those sub-projects and the FATAL_ERROR below would trigger.
list(APPEND CMAKE_TRY_COMPILE_PLATFORM_VARIABLES AARCH64_TOOLCHAIN_PATH)

# ----------------------------------------------------------------------------
# Resolve toolchain bin directory
# Priority:
#   1. Environment variable:  AARCH64_TOOLCHAIN_PATH=<path>  (default)
#   2. CMake cache variable:  -DAARCH64_TOOLCHAIN_PATH=<path> (override)
# ----------------------------------------------------------------------------
if(DEFINED ENV{AARCH64_TOOLCHAIN_PATH})
    set(_TC_BIN "$ENV{AARCH64_TOOLCHAIN_PATH}/bin")
    message(STATUS "AArch64 toolchain path (from environment): $ENV{AARCH64_TOOLCHAIN_PATH}")
elseif(DEFINED AARCH64_TOOLCHAIN_PATH)
    set(_TC_BIN "${AARCH64_TOOLCHAIN_PATH}/bin")
    message(STATUS "AArch64 toolchain path (from CMake variable): ${AARCH64_TOOLCHAIN_PATH}")
else()
    message(FATAL_ERROR
        "\n"
        "  AArch64 cross-compiler toolchain path not specified.\n"
        "\n"
        "  Provide the path to the ARM GNU Toolchain root directory using one of:\n"
        "\n"
        "    Environment variable (recommended):\n"
        "      export AARCH64_TOOLCHAIN_PATH=/path/to/arm-gnu-toolchain\n"
        "\n"
        "    CMake cache variable (override):\n"
        "      Uncomment AARCH64_TOOLCHAIN_PATH in qcperf/CMakeUserPresets.json\n"
        "      or pass: cmake ... -DAARCH64_TOOLCHAIN_PATH=/path/to/arm-gnu-toolchain\n"
        "\n"
        "  Example (ARM GNU Toolchain 15.2):\n"
        "      export AARCH64_TOOLCHAIN_PATH=/path/to/arm-gnu-toolchain-15.2.rel1-x86_64-aarch64-none-linux-gnu\n"
    )
endif()

# ----------------------------------------------------------------------------
# Validate the toolchain
# ----------------------------------------------------------------------------
if(NOT EXISTS "${_TC_BIN}/aarch64-none-linux-gnu-gcc")
    message(FATAL_ERROR
        "\n"
        "  Cross-compiler not found at: ${_TC_BIN}/aarch64-none-linux-gnu-gcc\n"
        "\n"
        "  Verify that AARCH64_TOOLCHAIN_PATH points to the toolchain ROOT directory\n"
        "  (the directory that contains the 'bin/' subdirectory).\n"
        "\n"
        "  Expected structure:\n"
        "    <AARCH64_TOOLCHAIN_PATH>/\n"
        "      bin/\n"
        "        aarch64-none-linux-gnu-gcc\n"
        "        aarch64-none-linux-gnu-g++\n"
    )
endif()

# ----------------------------------------------------------------------------
# Cross-compiler binaries
# ----------------------------------------------------------------------------
set(CMAKE_C_COMPILER   "${_TC_BIN}/aarch64-none-linux-gnu-gcc")
set(CMAKE_CXX_COMPILER "${_TC_BIN}/aarch64-none-linux-gnu-g++")

message(STATUS "AArch64 C   compiler: ${CMAKE_C_COMPILER}")
message(STATUS "AArch64 C++ compiler: ${CMAKE_CXX_COMPILER}")

# ----------------------------------------------------------------------------
# Search path configuration
# ----------------------------------------------------------------------------
# NEVER:  Do not search host machine for build tools (e.g. cmake find_program)
# ONLY:   Search only within the toolchain sysroot for libraries and headers
#         This prevents accidentally linking against host x86_64 libraries
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
