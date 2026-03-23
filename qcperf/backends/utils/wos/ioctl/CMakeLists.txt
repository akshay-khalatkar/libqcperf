# ============================================================================
# WOS IOCTL Utility Library
# ============================================================================
# This CMakeLists.txt builds the Windows on Snapdragon (WoS) IOCTL utility
# library, which provides a common interface for communicating with kernel-mode
# drivers via the Windows I/O Control (IOCTL) mechanism.
#
# This library is used by various backends to access hardware-specific
# information and control features that require kernel-mode access, such as
# power management, thermal control, and memory bandwidth monitoring.
#
# The library abstracts the complexities of Windows device driver communication,
# providing a simplified API for the backends to use.

add_library(WosUtilIoctl STATIC
    src/wos_ioctl.c
)

target_include_directories(WosUtilIoctl PUBLIC
    inc
)
