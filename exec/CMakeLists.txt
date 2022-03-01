add_executable(smplc-exec
    smplc.cpp
    ArgParse.cpp
)
set_target_properties(smplc-exec PROPERTIES OUTPUT_NAME "smplc")
target_link_libraries(smplc-exec
    smplc-wasm
)