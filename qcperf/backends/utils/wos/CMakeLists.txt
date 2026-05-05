# ============================================================================
# Windows on Snapdragon (WoS) Utilities
# ============================================================================
# This CMakeLists.txt manages utility libraries specific to Windows on Snapdragon
# platforms. These utilities provide interfaces to platform-specific features
# and monitoring capabilities that are used by the WoS backends.
#
# Currently includes:
# - ioctl: Interface for communicating with kernel-mode drivers via IOCTL
# - cpu-thermal-etw: Library for monitoring CPU thermal data via ETW (Event Tracing for Windows)
# - cpu-thermal-etw-test: Test application for the CPU thermal ETW library

# Add subdirectories for each utility
add_subdirectory(ioctl)
add_subdirectory(cpu-thermal-etw)
