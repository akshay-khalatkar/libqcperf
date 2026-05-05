# ============================================================================
# Version information configuration for QcPerf library
# ============================================================================

# Set default version if not provided via command line
if(NOT DEFINED ProjectVersion)
    set(ProjectVersion "0.1.0.0")
endif()

# Parse the version string in format build.major.minor.patch
string(REPLACE "." ";" VERSION_LIST ${ProjectVersion})
list(GET VERSION_LIST 0 VERSION_BUILD)
list(GET VERSION_LIST 1 VERSION_MAJOR)
list(GET VERSION_LIST 2 VERSION_MINOR)
list(GET VERSION_LIST 3 VERSION_PATCH)

# Create full version string
set(QCPERF_VERSION "${VERSION_BUILD}.${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}" PARENT_SCOPE)
set(VERSION_BUILD ${VERSION_BUILD} PARENT_SCOPE)
set(VERSION_MAJOR ${VERSION_MAJOR} PARENT_SCOPE)
set(VERSION_MINOR ${VERSION_MINOR} PARENT_SCOPE)
set(VERSION_PATCH ${VERSION_PATCH} PARENT_SCOPE)

# Get current timestamp in UTC
string(TIMESTAMP BUILD_TIMESTAMP "%Y-%m-%d %H:%M:%S" UTC)

# Try to get Git hash if Git is available
find_package(Git QUIET)
if(Git_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    if(NOT GIT_HASH)
        set(GIT_HASH "unknown")
    endif()
else()
    set(GIT_HASH "unknown")
endif()

# Configure version header
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version_info.h.in
    ${CMAKE_BINARY_DIR}/include/version_info.h
    @ONLY
)

# Add include directory for the generated header
include_directories(${CMAKE_BINARY_DIR}/include)

# Print version information
message(STATUS "")
message(STATUS "QcPerf Version Information:")
message(STATUS "  Version: ${VERSION_BUILD}.${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
message(STATUS "  Build timestamp: ${BUILD_TIMESTAMP}")
message(STATUS "  Git hash: ${GIT_HASH}")
message(STATUS "")

# Add compile definitions for version info
add_compile_definitions(
    LIBQCPERF_VERSION_MAJOR=${VERSION_MAJOR}
    LIBQCPERF_VERSION_MINOR=${VERSION_MINOR}
    LIBQCPERF_VERSION_PATCH=${VERSION_PATCH}
    LIBQCPERF_VERSION_BUILD=${VERSION_BUILD}
)

# Install the generated header
install(FILES ${CMAKE_BINARY_DIR}/include/version_info.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qcperf
)
