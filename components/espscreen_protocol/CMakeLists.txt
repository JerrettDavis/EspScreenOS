idf_component_register(
    SRCS "src/cobs.cpp" "src/frame.cpp" "src/rpc_router.cpp"
    INCLUDE_DIRS "include"
    REQUIRES espscreen_core
)
target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_20)
