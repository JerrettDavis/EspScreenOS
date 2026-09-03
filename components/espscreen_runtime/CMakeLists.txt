idf_component_register(
    SRCS "src/application_file_path.cpp" "src/firmware_system_restart.cpp" "src/firmware_runtime.cpp" "src/firmware_clock.cpp" "src/firmware_application.cpp" "src/firmware_applications.cpp" "src/firmware_builtin_applications.cpp" "src/firmware_capabilities.cpp" "src/firmware_services.cpp" "src/firmware_shell.cpp" "src/firmware_hardware_safety.cpp" "src/firmware_nvs_foundation.cpp" "src/firmware_lvgl_foundation.cpp" "src/firmware_lvgl_display.cpp" "src/firmware_shell_display.cpp" "src/firmware_shell_feedback.cpp" "src/firmware_shell_input.cpp" "src/firmware_diagnostics_source.cpp" "src/firmware_network_transport.cpp" "src/firmware_wifi_platform.cpp" "src/firmware_provisioning_server.cpp" "src/firmware_settings_backend.cpp" "src/firmware_asset_store.cpp" "src/firmware_application_jobs.cpp" "src/firmware_application_memory.cpp" "src/firmware_application_files.cpp"
    INCLUDE_DIRS "include"
    REQUIRES espscreen_core espscreen_hal espscreen_services espscreen_ui
    PRIV_REQUIRES espscreen_board espscreen_apps nvs_flash spiffs lvgl esp_wifi esp_netif esp_http_server freertos heap
)
target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_20)
