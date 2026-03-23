# ============================================================================
# QcPerf WOS Power Telemetry Library
# ============================================================================
# This CMakeLists.txt builds the Windows on Snapdragon (WoS) Power Telemetry
# library, which is a component of the WOS Power Backend. This library provides
# the core functionality for collecting power consumption metrics from various
# subsystems on Windows ARM64 platforms.
#
# The library interfaces with Windows Performance Data Helper (PDH) and
# Configuration Manager APIs to access power telemetry data from the system.
# It provides a unified interface for the power backend to collect and process
# power metrics.

add_library(QcWosPowerTelemetry STATIC
    src/power_telemetry.c
    ../src/wos_power_backend_logger.c
    ${CMAKE_SOURCE_DIR}/backends/utils/wos/ioctl/src/wos_ioctl.c
)

target_link_libraries(QcWosPowerTelemetry PUBLIC
    WosUtilIoctl
    pdh.lib
    Cfgmgr32.lib
)

target_include_directories(QcWosPowerTelemetry PUBLIC 
                            inc/
                            ${CMAKE_SOURCE_DIR}/backends/inc/
                            ${CMAKE_SOURCE_DIR}/backends/wos-power-backend/inc/
                            ${CMAKE_SOURCE_DIR}/backends/wos-power-backend/power-telemetry/inc/
                            ${CMAKE_SOURCE_DIR}/core/inc/
                            ${CMAKE_SOURCE_DIR}/backends/utils/wos/ioctl/inc/
                            ${CMAKE_SOURCE_DIR}/backends/utils/wos/pep/inc/
                            
                            )
