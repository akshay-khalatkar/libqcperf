# Windows OS Thermal Backend CMake configuration
# Builds the Windows-specific thermal monitoring backend for QcPerf

cmake_minimum_required(VERSION 3.13.5)

message("Added wos-thermal backend")

# Main thermal backend library
add_library(QcPerfWosThermalBackend STATIC
    src/wos_thermal.c # Main thermal backend implementation
    src/wos_thermal_info.c # Thermal information provider
    src/wos_thermal_lib.c # Thermal library utilities
    src/wos_thermal_logger.c # Thermal logging functionality
)

# Include directories for thermal backend
target_include_directories(QcPerfWosThermalBackend PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc # Local includes
    ${CMAKE_SOURCE_DIR}/core/inc # Core QcPerf includes
    ${CMAKE_SOURCE_DIR}/core/inc/internal # Internal QcPerf includes
    ${CMAKE_SOURCE_DIR}/backends/inc # Backend common includes
    ${CMAKE_SOURCE_DIR}/backends/utils/wos/ioctl/inc # Windows IOCTL utilities
    ${CMAKE_SOURCE_DIR}/backends/utils/wos/pep/inc # Windows PEP utilities
    ${CMAKE_SOURCE_DIR}/utils/qthread/inc # Thread utilities
    ${CMAKE_SOURCE_DIR}/utils/qtime/inc # Time utilities
)

# Link with required libraries
target_link_libraries(QcPerfWosThermalBackend PUBLIC
    QcPerfQThread # Thread utilities
    WosUtilIoctl # Windows IOCTL utilities
    Cfgmgr32.lib # Windows Configuration Manager library
    tdh.lib # Windows Trace Data Helper library
    QcPerfWosCpuThermalEtw # CPU thermal ETW monitoring
    QcPerfWosPassiveCooling # Passive cooling monitoring
    QcPerfWosThermalEtwCommon # Common thermal ETW utilities
    QcPerfQTime # Time utilities
)
