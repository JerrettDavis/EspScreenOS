idf_component_register(
    SRCS
        "src/dcs_panel.cpp"
        "src/display_device.cpp"
        "src/i2c_bus.cpp"
        "src/touch_device.cpp"
        "src/touch_input_filter.cpp"
        "src/touch_geometry.cpp"
        "src/storage_device.cpp"
        "src/storage_mount_policy.cpp"
        "src/power_device.cpp"
        "src/power_measurement.cpp"
        "src/audio_device.cpp"
        "src/esp_i2s_audio_transport.cpp"
        "src/es8311_codec.cpp"
        "src/audio_signal.cpp"
        "src/haptics_controller.cpp"
        "src/board_resource_plan.cpp"
        "src/hardware_platform.cpp"
        "src/application_hardware_adapters.cpp"
    INCLUDE_DIRS "include"
    PRIV_INCLUDE_DIRS "src"
    REQUIRES espscreen_core espscreen_board espscreen_resources
    PRIV_REQUIRES esp_lcd esp_lcd_st77922 driver esp_adc esp_timer fatfs sdmmc heap
)
target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_20)
