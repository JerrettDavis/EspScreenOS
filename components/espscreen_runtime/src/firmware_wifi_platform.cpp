#include "espscreen/runtime/firmware_wifi_platform.hpp"
#include "esp_event.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include <cstdio>
#include <cstring>

namespace espscreen::runtime {

core::Status FirmwareWifiPlatform::start(IWifiPlatformEvents& events) {
    const auto transition = lifecycle_.begin_start("Wi-Fi platform already active");
    if (!transition.status) return transition.status;
    if (!transition.acquire)
        return events_.load() == &events
                   ? core::Status::success()
                   : core::Status{core::ErrorCode::conflict,
                                  "Wi-Fi platform already active"};
    events_.store(&events);
    if (esp_netif_init() != ESP_OK) {
        stop();
        return {core::ErrorCode::io_error, "network stack failed"};
    }
    const auto event_loop = esp_event_loop_create_default();
    if (event_loop != ESP_OK && event_loop != ESP_ERR_INVALID_STATE) {
        stop();
        return {core::ErrorCode::io_error, "event loop failed"};
    }
    wifi_init_config_t configuration = WIFI_INIT_CONFIG_DEFAULT();
    if (esp_wifi_init(&configuration) != ESP_OK) {
        stop();
        return {core::ErrorCode::io_error, "Wi-Fi initialization failed"};
    }
    wifi_initialized_ = true;
    if (const auto registered = register_event_handlers(); !registered) {
        stop();
        return registered;
    }
    lifecycle_.started();
    return core::Status::success();
}

core::Status FirmwareWifiPlatform::register_event_handlers() {
    esp_event_handler_instance_t wifi_handler{};
    if (esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler,
                                            this, &wifi_handler) != ESP_OK)
        return {core::ErrorCode::io_error, "Wi-Fi event registration failed"};
    wifi_event_handler_ = wifi_handler;
    esp_event_handler_instance_t ip_handler{};
    if (esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler,
                                            this, &ip_handler) != ESP_OK) {
        unregister_event_handlers();
        return {core::ErrorCode::io_error, "IP event registration failed"};
    }
    ip_event_handler_ = ip_handler;
    return core::Status::success();
}

void FirmwareWifiPlatform::unregister_event_handlers() {
    if (ip_event_handler_) {
        (void)esp_event_handler_instance_unregister(
            IP_EVENT, IP_EVENT_STA_GOT_IP,
            static_cast<esp_event_handler_instance_t>(ip_event_handler_));
        ip_event_handler_ = nullptr;
    }
    if (wifi_event_handler_) {
        (void)esp_event_handler_instance_unregister(
            WIFI_EVENT, ESP_EVENT_ANY_ID,
            static_cast<esp_event_handler_instance_t>(wifi_event_handler_));
        wifi_event_handler_ = nullptr;
    }
}

core::Status FirmwareWifiPlatform::start_station(std::string_view ssid,
                                                 std::string_view password) {
    if (!lifecycle_.running())
        return {core::ErrorCode::conflict, "Wi-Fi platform is not ready"};
    if (!station_netif_) station_netif_ = esp_netif_create_default_wifi_sta();
    if (!station_netif_)
        return {core::ErrorCode::unavailable, "station network interface failed"};
    wifi_config_t configuration{};
    std::snprintf(reinterpret_cast<char*>(configuration.sta.ssid),
                  sizeof(configuration.sta.ssid), "%.*s", static_cast<int>(ssid.size()),
                  ssid.data());
    std::snprintf(reinterpret_cast<char*>(configuration.sta.password),
                  sizeof(configuration.sta.password), "%.*s",
                  static_cast<int>(password.size()), password.data());
    configuration.sta.threshold.authmode =
        password.empty() ? WIFI_AUTH_OPEN : WIFI_AUTH_WPA2_PSK;
    if (esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK ||
        esp_wifi_set_config(WIFI_IF_STA, &configuration) != ESP_OK ||
        esp_wifi_start() != ESP_OK || esp_wifi_connect() != ESP_OK)
        return {core::ErrorCode::io_error, "station startup failed"};
    return core::Status::success();
}

core::Status FirmwareWifiPlatform::start_access_point() {
    if (!lifecycle_.running())
        return {core::ErrorCode::conflict, "Wi-Fi platform is not ready"};
    (void)esp_wifi_stop();
    if (station_netif_) {
        esp_netif_destroy_default_wifi(static_cast<esp_netif_t*>(station_netif_));
        station_netif_ = nullptr;
    }
    if (!access_point_netif_) access_point_netif_ = esp_netif_create_default_wifi_ap();
    if (!access_point_netif_)
        return {core::ErrorCode::unavailable, "provisioning network interface failed"};
    std::uint8_t mac[6]{};
    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);
    wifi_config_t configuration{};
    std::snprintf(reinterpret_cast<char*>(configuration.ap.ssid),
                  sizeof(configuration.ap.ssid), "EspScreen-%02X%02X", mac[4], mac[5]);
    configuration.ap.ssid_len =
        std::strlen(reinterpret_cast<char*>(configuration.ap.ssid));
    configuration.ap.channel = 1;
    configuration.ap.max_connection = 4;
    configuration.ap.authmode = WIFI_AUTH_OPEN;
    if (esp_wifi_set_mode(WIFI_MODE_AP) != ESP_OK ||
        esp_wifi_set_config(WIFI_IF_AP, &configuration) != ESP_OK ||
        esp_wifi_start() != ESP_OK)
        return {core::ErrorCode::io_error, "provisioning AP failed"};
    return core::Status::success();
}

void FirmwareWifiPlatform::reconnect() {
    if (lifecycle_.running()) (void)esp_wifi_connect();
}

void FirmwareWifiPlatform::event_handler(void* context, const char* event_base,
                                         std::int32_t event_id, void* event_data) {
    auto& self = *static_cast<FirmwareWifiPlatform*>(context);
    if (!self.lifecycle_.running()) return;
    auto* events = self.events_.load();
    if (!events) return;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        events->wifi_disconnected();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        const auto& event = *static_cast<ip_event_got_ip_t*>(event_data);
        char address[16]{};
        std::snprintf(address, sizeof(address), IPSTR, IP2STR(&event.ip_info.ip));
        events->wifi_connected(address);
    }
}

void FirmwareWifiPlatform::destroy_netifs() {
    if (station_netif_)
        esp_netif_destroy_default_wifi(static_cast<esp_netif_t*>(station_netif_));
    if (access_point_netif_)
        esp_netif_destroy_default_wifi(static_cast<esp_netif_t*>(access_point_netif_));
    station_netif_ = access_point_netif_ = nullptr;
}

void FirmwareWifiPlatform::stop() {
    if (!lifecycle_.begin_stop()) return;
    events_.store(nullptr);
    unregister_event_handlers();
    if (wifi_initialized_) {
        (void)esp_wifi_stop();
        (void)esp_wifi_deinit();
        wifi_initialized_ = false;
    }
    destroy_netifs();
    lifecycle_.stopped();
}

} // namespace espscreen::runtime
