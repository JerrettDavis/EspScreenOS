idf_component_register(
    SRCS "app_main.cpp"
    INCLUDE_DIRS "."
    PRIV_REQUIRES espscreen_runtime freertos log
)
target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_20)
