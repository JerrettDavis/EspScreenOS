cmake_minimum_required(VERSION 3.20)
project(espscreen_host_tests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(espscreen_host_tests
    test_main.cpp
    ../../components/espscreen_hal/src/touch_input_filter.cpp
    ../../components/espscreen_hal/src/touch_geometry.cpp
    ../../components/espscreen_hal/src/power_measurement.cpp
    ../../components/espscreen_hal/src/storage_mount_policy.cpp
    ../../components/espscreen_hal/src/audio_signal.cpp
    ../../components/espscreen_core/src/service_manager.cpp
    ../../components/espscreen_core/src/app_manager.cpp
    ../../components/espscreen_core/src/runtime_loop.cpp
    ../../components/espscreen_core/src/startup_plan.cpp
    ../../components/espscreen_services/src/diagnostics_service.cpp
    ../../components/espscreen_services/src/settings_service.cpp
    ../../components/espscreen_services/src/asset_service.cpp
    ../../components/espscreen_services/src/wifi_credentials.cpp
    ../../components/espscreen_services/src/wifi_settings.cpp
    ../../components/espscreen_services/src/network_connection_policy.cpp
    ../../components/espscreen_services/src/network_service.cpp
    ../../components/espscreen_apps/src/calculator_engine.cpp
    ../../components/espscreen_apps/src/calculator_app.cpp
    ../../components/espscreen_apps/src/hardware_test_model.cpp
    ../../components/espscreen_apps/src/hardware_test_app.cpp
    ../../components/espscreen_apps/src/gpio_toolbox_app.cpp
    ../../components/espscreen_apps/src/notes_app.cpp
    ../../components/espscreen_apps/src/paint_app.cpp
    ../../components/espscreen_apps/src/recorder_app.cpp
    ../../components/espscreen_apps/src/dashboard_model.cpp
    ../../components/espscreen_apps/src/dashboard_app.cpp
    ../../components/espscreen_apps/src/board_info_model.cpp
    ../../components/espscreen_apps/src/board_info_app.cpp
    ../../components/espscreen_apps/src/sd_app_path.cpp
    ../../components/espscreen_apps/src/sd_app_loader.cpp
    ../../components/espscreen_apps/src/external_application.cpp
    ../../components/espscreen_runtime/src/application_file_path.cpp
    ../../components/espscreen_runtime/src/firmware_application_files.cpp
    ../../components/espscreen_runtime/src/firmware_network_transport.cpp
    ../../components/espscreen_resources/src/resource_broker.cpp
    ../../components/espscreen_hal/src/board_resource_plan.cpp
    ../../components/espscreen_protocol/src/cobs.cpp
    ../../components/espscreen_protocol/src/frame.cpp
    ../../components/espscreen_protocol/src/rpc_router.cpp
    ../../components/espscreen_expansion/src/expansion_manager.cpp
    ../../components/espscreen_ui/src/display_power_policy.cpp
    ../../components/espscreen_ui/src/shell_settings.cpp
    ../../components/espscreen_ui/src/shell_settings_persistence.cpp
    ../../components/espscreen_ui/src/shell_input_policy.cpp
    ../../components/espscreen_ui/src/shell_action_policy.cpp
    ../../components/espscreen_ui/src/shell.cpp
)
target_include_directories(espscreen_host_tests PRIVATE
    ../../components/espscreen_core/include
    ../../components/espscreen_board/include
    ../../components/espscreen_resources/include
    ../../components/espscreen_protocol/include
    ../../components/espscreen_expansion/include
    ../../components/espscreen_hal/include
    ../../components/espscreen_ui/include
    ../../components/espscreen_services/include
    ../../components/espscreen_apps/include
    ../../components/espscreen_runtime/include
)
if(MSVC)
    target_compile_options(espscreen_host_tests PRIVATE /W4 /WX)
else()
    target_compile_options(espscreen_host_tests PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()
enable_testing()
add_test(NAME espscreen_host_tests COMMAND espscreen_host_tests)
