# CPU Thermal ETW library CMake configuration
# Builds Windows-specific CPU thermal monitoring libraries using ETW (Event Tracing for Windows)

cmake_minimum_required(VERSION 3.11)

# Needed for krabs compilation (ETW library requires Unicode support and C++17)
add_compile_definitions(UNICODE _UNICODE)

# Set C++17 standard for this component
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Only build these libraries on Windows ARM64 platforms
if(QCPERF_OS_WINDOWS AND QCPERF_ARCH_ARM64)
    # Common thermal monitoring utilities
    add_library(QcPerfWosThermalEtwCommon STATIC
        src/thermal_common.cpp
    )

    # Link with required utility libraries
    target_link_libraries(QcPerfWosThermalEtwCommon PUBLIC
        QcPerfQThread
        QcPerfQMutex
        QcPerfQSleep
        QcPerfQCv
        QcPerfqlist
    )

    # Include directories for ThermalEtwCommon
    target_include_directories(QcPerfWosThermalEtwCommon PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/
        ${CMAKE_SOURCE_DIR}/third-party/krabs/krabs
        ${CMAKE_SOURCE_DIR}/utils/qthread/inc
        ${CMAKE_SOURCE_DIR}/utils/qmutex/inc
        ${CMAKE_SOURCE_DIR}/utils/qsleep/inc
        ${CMAKE_SOURCE_DIR}/utils/qcondition-variable/inc
        ${CMAKE_SOURCE_DIR}/utils/qlist/inc
    )

    # CPU thermal ETW monitoring library
    add_library(QcPerfWosCpuThermalEtw STATIC
        src/cpu_thermal_etw_library.cpp
    )

    # Link with Windows-specific libraries and utility libraries
    target_link_libraries(QcPerfWosCpuThermalEtw PUBLIC
        pdh.lib                    # Windows Performance Data Helper library
        wbemuuid.lib               # Windows Management Instrumentation library
        QcPerfQThread
        QcPerfQMutex
        QcPerfQSleep
        QcPerfWosThermalEtwCommon
    )

    # Include directories for CpuThermalEtw
    target_include_directories(QcPerfWosCpuThermalEtw PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/
        ${CMAKE_SOURCE_DIR}/third-party/krabs/krabs
        ${CMAKE_SOURCE_DIR}/utils/qthread/inc
        ${CMAKE_SOURCE_DIR}/utils/qmutex/inc
        ${CMAKE_SOURCE_DIR}/utils/qsleep/inc
        ${CMAKE_SOURCE_DIR}/utils/qcondition-variable/inc
    )

    # Passive cooling monitoring library
    add_library(QcPerfWosPassiveCooling STATIC
        src/passive_cooling.c
    )

    # Link with Windows-specific libraries and utility libraries
    target_link_libraries(QcPerfWosPassiveCooling PUBLIC
        pdh.lib                    # Windows Performance Data Helper library
        QcPerfQThread
        QcPerfQMutex
        QcPerfQSleep
        QcPerfWosThermalEtwCommon
    )

    # Include directories for PassiveCooling
    target_include_directories(QcPerfWosPassiveCooling PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/inc/
        ${CMAKE_SOURCE_DIR}/third-party/krabs/krabs
        ${CMAKE_SOURCE_DIR}/utils/qthread/inc
        ${CMAKE_SOURCE_DIR}/utils/qmutex/inc
        ${CMAKE_SOURCE_DIR}/utils/qsleep/inc
        ${CMAKE_SOURCE_DIR}/utils/qcondition-variable/inc
    )
endif()
