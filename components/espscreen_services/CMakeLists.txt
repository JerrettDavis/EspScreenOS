idf_component_register(
    SRCS
        "src/settings_service.cpp"
        "src/asset_service.cpp"
        "src/diagnostics_service.cpp"
        "src/network_service.cpp"
        "src/wifi_credentials.cpp"
        "src/wifi_settings.cpp"
        "src/network_connection_policy.cpp"
    INCLUDE_DIRS "include"
    REQUIRES espscreen_core
)
target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_20)
