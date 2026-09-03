idf_component_register(
    SRCS "src/expansion_manager.cpp"
    INCLUDE_DIRS "include"
    REQUIRES espscreen_core espscreen_board espscreen_resources
)
target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_20)
