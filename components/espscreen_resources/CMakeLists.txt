idf_component_register(
    SRCS "src/resource_broker.cpp"
    INCLUDE_DIRS "include"
    REQUIRES espscreen_core
)
target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_20)
