add_library(QcPerfQcomDsp STATIC
    src/qcom_dsp.c
    src/dspquery_stub.c
    
)

add_library(cdsprpc SHARED
    src/cdsprpc_stub.c
    
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