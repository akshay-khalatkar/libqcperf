# ============================================================================
# QcPerf - Windows Platform Configuration
# ============================================================================
# This file contains Windows-specific build configurations for the QcPerf library.
# It sets compiler flags, preprocessor definitions, and library dependencies
# specific to Windows platforms, with special handling for different architectures
# (ARM64, x86_64).
#
# This file is included by BuildConfig.cmake when building on Windows platforms.
# ============================================================================

# ============================================================================
# Windows Platform Definitions
# ============================================================================
# Enable Windows-specific preprocessor definitions for conditional compilation
add_compile_definitions(QCPERF_PLATFORM_WINDOWS)

# ============================================================================
# Windows Compiler Configuration
# ============================================================================
# Configure compiler flags specific to Microsoft Visual C++ (MSVC)
if(MSVC)
    # Enable multi-processor compilation
    add_compile_options(/MP)

    # Warning level
    add_compile_options(/W4)

    # Disable specific warnings if needed
    # add_compile_options(/wd4100) # Unused parameter

    # Enable additional security features
    add_compile_definitions(
        _CRT_SECURE_NO_WARNINGS
        _SCL_SECURE_NO_WARNINGS
    )
endif()

# ============================================================================
# Windows ARM64 Configuration
# ============================================================================
# Special handling for Windows on ARM64 architecture (e.g., Surface Pro X)
if(QCPERF_PLATFORM_WINDOWS_ARM64)
    message(STATUS "Configuring for Windows ARM64")

    # Add ARM64-specific compile definitions
    add_compile_definitions(WIN32_ARM64)


    if(MSVC)
        # ARM64-specific optimizations
        # Note: Visual Studio doesn't support /arch:armv8.0 for ARM64
        # It automatically uses the appropriate architecture

        # Additional ARM64-specific optimizations can be added here
    endif()
    setcompileoptions(_WIN_ARM64)
endif()

# ============================================================================
# Windows x86_64 Configuration
# ============================================================================
# Configuration for standard 64-bit Intel/AMD platforms
if(QCPERF_PLATFORM_WINDOWS_X86_64)
    message(STATUS "Configuring for Windows x86_64")

    # Add x86_64-specific compile definitions if needed
    # add_compile_definitions(WIN32_X64)

    if(MSVC)
        # Enable x86_64 specific optimizations if needed
        # add_compile_options(/arch:AVX2)
    endif()
    setcompileoptions(_WIN_x86_64)
endif()

# ============================================================================
# Windows Library Dependencies
# ============================================================================
# Define Windows-specific libraries that need to be linked with the project
set(QCPERF_WINDOWS_LIBS "")

# Add Windows-specific libraries if needed
if(QCPERF_OS_WINDOWS)
    list(APPEND QCPERF_WINDOWS_LIBS
        # Add Windows-specific libraries here
        # Example: ws2_32 (Windows Sockets)
    )
endif()

set(_OS_M "")
set(_OS_THREAD "")

message(STATUS "Windows platform configuration loaded")
