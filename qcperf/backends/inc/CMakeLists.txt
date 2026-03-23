# ============================================================================
# QcPerf Backend Interface Library
# ============================================================================
# This CMakeLists.txt defines the unified backend interface library that
# aggregates all backend implementations and provides a single point of
# dependency for the core library. It centralizes backend-specific includes
# and dependencies, allowing the core to remain decoupled from specific backends.

# ============================================================================
# Interface Library Target
# ============================================================================
# Create a unified backend interface library
# This is an INTERFACE library (header-only) that serves as a facade
# for all backend implementations
add_library(QcPerfBackends INTERFACE)

# ============================================================================
# Include Directories
# ============================================================================
# Set include directories for the unified backend interface
# These are the common headers that all backends need to implement
target_include_directories(QcPerfBackends INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/backends/dummy/inc
)

# ============================================================================
# Dependencies
# ============================================================================
# Link against all backend libraries
# The dummy backend is always included as a fallback
target_link_libraries(QcPerfBackends INTERFACE
    QcPerfDummyBackend
)

# ============================================================================
# Platform-Specific Configuration
# ============================================================================
# Add platform-specific backends
# Windows ARM64 platforms include additional backends for hardware monitoring
if(QCPERF_PLATFORM_WINDOWS_ARM64)
    target_include_directories(QcPerfBackends INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/backends/wos-thermal/inc
        ${CMAKE_SOURCE_DIR}/backends/wos-power-backend/inc
    )

    target_link_libraries(QcPerfBackends INTERFACE
        QcPerfWosThermalBackend
        QcPerfWosPowerBackend
    )

endif()
