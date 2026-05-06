# Create the main QcPerfCoreTest library
add_executable(QcPerfCoreTest 
    src/qcperf_core_test.c
)

# Set target properties
set_target_properties(QcPerfCoreTest PROPERTIES
    OUTPUT_NAME "QcPerfCoreTest"
    POSITION_INDEPENDENT_CODE OFF
)

# Include directories for the core library
target_include_directories(QcPerfCoreTest
    PUBLIC
        ${CMAKE_SOURCE_DIR}/core/inc
)

# Link dependencies
target_link_libraries(QcPerfCoreTest
    PUBLIC
        QcPerfCore
)
if(NOT BUILD_SHARED AND QCPERF_LINUX_ARM64 AND QCPERF_ENABLED_QCOM_LINUX_NPU)
    target_link_libraries(QcPerfCoreTest PRIVATE
        cdsprpc
    )
endif()

# Compiler-specific options for the core library
if(MSVC)
    target_compile_options(QcPerfCoreTest PRIVATE
        /W4
        /WX-  # Don't treat warnings as errors (can be changed to /WX if needed)
    )
else()
    target_compile_options(QcPerfCoreTest PRIVATE
        -Wall
        -Wextra
        -Wpedantic
    )
endif()
