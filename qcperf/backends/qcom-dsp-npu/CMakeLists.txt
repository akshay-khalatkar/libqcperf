# ============================================================================
# QcPerf DSP NPU Backend
# ============================================================================
# This CMakeLists.txt builds the DSP NPU backend, which collects real
# performance metrics from Qualcomm NPU/CDSP hardware using the DSP library.
# The DSP NPU backend implements the standard backend interface defined in
# qcperf/backends/inc, allowing it to be used interchangeably with other
# backends through the unified backend interface.

message("Added DSP NPU backend")

# Create the DSP NPU backend library
add_library(QcPerfDspNpuBackend STATIC
    src/qcom_dsp_npu.c
    src/qcom_dsp_npu_info.c
)

target_include_directories(QcPerfDspNpuBackend PUBLIC 
    inc
    ${CMAKE_SOURCE_DIR}/core/inc
    ${CMAKE_SOURCE_DIR}/core/inc/internal
    ${CMAKE_SOURCE_DIR}/backends/inc
    ${CMAKE_SOURCE_DIR}/backends/qcom-dsp/inc
    ${CMAKE_SOURCE_DIR}/utils/qthread/inc
    ${CMAKE_SOURCE_DIR}/utils/qtime/inc
)

target_link_libraries(QcPerfDspNpuBackend PUBLIC
    QcPerfQThread
    QcPerfQTime
    ${_OS_M}
)

# Link the DSP library for NPU access
target_link_libraries(QcPerfDspNpuBackend PRIVATE
    QcPerfQcomDsp
)