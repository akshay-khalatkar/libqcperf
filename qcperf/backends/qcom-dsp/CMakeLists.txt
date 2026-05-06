add_library(QcPerfQcomDsp STATIC
    src/qcom_dsp.c
    src/dspquery_stub.c
)

# libcdsprpc.so and its headers are Qualcomm BSP runtime dependencies
# provided by the target device — they are NOT built or bundled by this project.

find_path(FASTRPC_INCLUDE_DIR AEEStdDef.h
    PATHS
        /usr/include
        /usr/local/include/fastrpc
        /usr/include/aarch64-linux-gnu
)

if(NOT FASTRPC_INCLUDE_DIR)
    message(FATAL_ERROR
        "FastRPC headers not found (AEEStdDef.h). "
        "Install the Qualcomm BSP headers to /usr/include or "
        "/usr/local/include/fastrpc on the target device."
    )
endif()

find_library(CDSPRPC_LIBRARY NAMES cdsprpc
    PATHS
        /usr/lib
        /usr/local/lib
        /usr/lib/aarch64-linux-gnu
)

if(NOT CDSPRPC_LIBRARY)
    message(FATAL_ERROR
        "libcdsprpc.so not found. "
        "Install the Qualcomm BSP libraries to /usr/lib or "
        "/usr/lib/aarch64-linux-gnu on the target device."
    )
endif()

target_include_directories(QcPerfQcomDsp PUBLIC
    inc
    ${FASTRPC_INCLUDE_DIR}
)

target_link_libraries(QcPerfQcomDsp PRIVATE
    ${CDSPRPC_LIBRARY}
)