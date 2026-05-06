# ============================================================================
# QcPerf Qualcomm Linux CPU Backend
# ============================================================================
# This CMakeLists.txt builds the qcom-linux-cpu backend, which monitors CPU
# performance metrics on Qualcomm Linux platforms

message("Added qcom-linux-cpu backend")

add_library(QcPerfQcomLinuxCpuBackend STATIC
    src/qcom_linux_cpu.c
    src/qcom_linux_cpu_info.c
    src/qcom_linux_cpu_utils.c
)

target_include_directories(QcPerfQcomLinuxCpuBackend PUBLIC
    inc
    ${CMAKE_SOURCE_DIR}/core/inc
    ${CMAKE_SOURCE_DIR}/core/inc/internal
    ${CMAKE_SOURCE_DIR}/backends/inc
    ${CMAKE_SOURCE_DIR}/utils/qthread/inc
    ${CMAKE_SOURCE_DIR}/utils/qtime/inc
)

target_link_libraries(QcPerfQcomLinuxCpuBackend
    QcPerfQThread
    QcPerfQTime
    ${_OS_M}
)
