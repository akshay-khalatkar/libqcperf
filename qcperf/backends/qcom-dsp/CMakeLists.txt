add_library(QcPerfQcomDsp STATIC
    src/qcom_dsp.c
    src/dspquery_stub.c
    
)

add_library(cdsprpc SHARED
    src/cdsprpc_stub.c

)

# libcdsprpc.so is a BSP runtime dependency on the target device.
# This stub exists only to satisfy the linker at build time; it must not
# be included in the deliverable package.  Redirect it to a separate
# directory so it cannot be mistaken for a real output artifact.
set_target_properties(cdsprpc PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/link-stubs"
)

target_include_directories(QcPerfQcomDsp PUBLIC
    inc
)

target_include_directories(cdsprpc PUBLIC
    inc
)

target_link_libraries(QcPerfQcomDsp PUBLIC 
    cdsprpc
)