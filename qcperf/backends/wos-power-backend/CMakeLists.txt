# ============================================================================
# QcPerf WOS Power Backend
# ============================================================================
# This CMakeLists.txt builds the Windows on Snapdragon (WoS) Power
# backend for the QcPerf library. This backend monitors power consumption
# metrics for various subsystems including CPU clusters, GPU, and system-level
# power metrics.
#
# The backend interfaces with Windows-specific power telemetry APIs to collect
# real-time power consumption data, providing metrics such as:
# - Power consumption for individual CPU clusters
# - GPU power consumption
# - System-level power metrics (PSU, USB, etc.)
#
# This backend is only built for Windows ARM64 platforms.

message("Added wos-power-backend backend")

add_library(QcPerfWosPowerBackend STATIC
    src/wos_power_backend.c
    ${CMAKE_SOURCE_DIR}/backends/utils/wos/ioctl/src/wos_ioctl.c
)

add_subdirectory(power-telemetry)

target_include_directories(QcPerfWosPowerBackend PUBLIC
    inc
    power-telemetry/inc
    ${CMAKE_SOURCE_DIR}/backends/inc/
    ${CMAKE_SOURCE_DIR}/core/inc
    ${CMAKE_SOURCE_DIR}/core/inc/internal/
    ${CMAKE_SOURCE_DIR}/backends/utils/wos/ioctl/inc/
    ${CMAKE_SOURCE_DIR}/utils/qthread/inc
    ${CMAKE_SOURCE_DIR}/utils/qtime/inc
)

target_link_libraries(QcPerfWosPowerBackend
    QcPerfQThread
    QcPerfQTime
    QcWosPowerTelemetry
    WosUtilIoctl
)
