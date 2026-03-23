# ============================================================================
# QcPerf Backend Utilities
# ============================================================================
# This CMakeLists.txt manages utility libraries used by backend implementations.
# These utilities provide common functionality that can be shared across
# multiple backends, such as platform-specific monitoring interfaces.
#
# Currently includes:
# - wos: Windows on Snapdragon specific utilities for thermal monitoring,
#        IOCTL operations, and other platform-specific functionality

# ============================================================================
# Platform-Specific Utilities
# ============================================================================
# Add Windows on Snapdragon (WoS) utilities when building for Windows ARM64
if(QCPERF_PLATFORM_WINDOWS_ARM64)
    add_subdirectory(wos)
endif()
