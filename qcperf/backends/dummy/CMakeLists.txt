# ============================================================================
# QcPerf Dummy Backend
# ============================================================================
# This CMakeLists.txt builds the dummy backend, which serves as a reference
# implementation and fallback for the QcPerf library. The dummy backend
# generates synthetic performance metrics without requiring actual hardware
# monitoring capabilities, making it useful for testing and development.
#
# The dummy backend implements the standard backend interface defined in
# qcperf/backends/inc, allowing it to be used interchangeably with other
# backends through the unified backend interface.

# Create the dummy backend library
add_library(QcPerfDummyBackend STATIC
    src/dummy.c
    src/dummy_info.c
)

target_include_directories(QcPerfDummyBackend PUBLIC 
    inc
    ${CMAKE_SOURCE_DIR}/core/inc
    ${CMAKE_SOURCE_DIR}/core/inc/internal
    ${CMAKE_SOURCE_DIR}/backends/inc
    ${CMAKE_SOURCE_DIR}/utils/qthread/inc
    ${CMAKE_SOURCE_DIR}/utils/qtime/inc
)

target_link_libraries(QcPerfDummyBackend
    QcPerfQThread
    QcPerfQTime
    ${_OS_M}
)
