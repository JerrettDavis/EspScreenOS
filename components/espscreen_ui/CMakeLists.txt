idf_component_register(
    SRCS "src/shell.cpp" "src/shell_view.cpp" "src/shell_action_policy.cpp" "src/shell_settings.cpp" "src/shell_settings_persistence.cpp" "src/shell_input_policy.cpp" "src/header_view.cpp" "src/status_view.cpp" "src/navigation_view.cpp" "src/settings_view.cpp" "src/drawer_view.cpp" "src/motion.cpp" "src/display_power_policy.cpp"
    INCLUDE_DIRS "include"
    REQUIRES espscreen_core
    PRIV_REQUIRES lvgl
)
target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_20)
