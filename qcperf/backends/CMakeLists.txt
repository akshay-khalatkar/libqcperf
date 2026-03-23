# ============================================================================
# QcPerf Backends
# ============================================================================
# This CMakeLists.txt manages all backend implementations for the QcPerf library.
# It includes individual backend subdirectories and sets up the unified backend
# interface that the core library depends on.
#
# The QcPerf library uses a modular backend architecture where each backend
# provides specific performance monitoring capabilities:
#
# - dummy: A reference implementation that generates synthetic metrics for testing
# - wos-thermal: Monitors temperature and passive cooling for various thermal zones
# - wos-power-backend: Monitors power consumption metrics
#
# Each backend implements a common interface defined in the inc/ directory,
# allowing the core library to interact with any backend in a uniform way.
# Platform-specific backends are conditionally included based on the target platform.

# ============================================================================
# Backend Subdirectories
# ============================================================================
# Add subdirectories for individual backends
# The dummy backend is always included as a fallback implementation
add_subdirectory(dummy)

# Add utility libraries used by backends
add_subdirectory(utils)

# Add platform-specific backends
# Windows ARM64 platforms include additional backends for hardware monitoring
if(QCPERF_PLATFORM_WINDOWS_ARM64)
    add_subdirectory(wos-thermal)
    add_subdirectory(wos-power-backend)
endif()

# ============================================================================
# Backend Interface
# ============================================================================
# Add the inc directory which contains the unified backend interface
# This creates an interface library that aggregates all backends
add_subdirectory(inc)
