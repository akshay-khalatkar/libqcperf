# Test application using Unity Testing Framework

# Handle Unity library
if(NOT TARGET unity)
    if(EXISTS "${CMAKE_SOURCE_DIR}/third-party/Unity/src/unity.c")
        add_library(unity STATIC
            ${CMAKE_SOURCE_DIR}/third-party/Unity/src/unity.c
        )
        target_include_directories(unity PUBLIC
            ${CMAKE_SOURCE_DIR}/third-party/Unity/src
        )
    else()
        message(WARNING "Unity framework not configured")
        return()
    endif()
endif()

# Test executable for core functionality
add_executable(test_qcperf_core
    src/test_runner.c
    src/test_qcperf_unit.c
)

target_include_directories(test_qcperf_core PRIVATE
    ${CMAKE_SOURCE_DIR}/third-party/Unity/src
    ${CMAKE_SOURCE_DIR}/core/inc
    ${CMAKE_SOURCE_DIR}/backends/inc
    ${CMAKE_SOURCE_DIR}/backends/dummy/inc
)

target_link_libraries(test_qcperf_core
    PRIVATE
    unity
    QcPerfCore
)

add_test(
    NAME qcperf_core_tests
    COMMAND test_qcperf_core
)

# Test executable for backends
add_executable(test_qcperf_backends
    src/test_runner_backends.c
    src/test_backends.c
)

target_include_directories(test_qcperf_backends PRIVATE
    ${CMAKE_SOURCE_DIR}/third-party/Unity/src
    ${CMAKE_SOURCE_DIR}/backends/inc
    ${CMAKE_SOURCE_DIR}/backends/dummy/inc
    ${CMAKE_SOURCE_DIR}/core/inc
)

target_link_libraries(test_qcperf_backends
    PRIVATE
    unity
    QcPerfCore
    QcPerfDummyBackend
)

add_test(
    NAME qcperf_backends_tests
    COMMAND test_qcperf_backends
)