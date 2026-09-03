idf_component_register(
    SRCS "src/service_manager.cpp" "src/app_manager.cpp" "src/runtime_loop.cpp" "src/startup_plan.cpp"
    INCLUDE_DIRS "include"
)
target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_20)
