#pragma once

#include "espscreen/core/atomic_component_lifecycle.hpp"
#include "espscreen/runtime/wifi_platform.hpp"
#include <atomic>
#include <cstdint>

namespace espscreen::runtime {

class FirmwareWifiPlatform final : public IWifiPlatform {
public:
    [[nodiscard]] core::Status start(IWifiPlatformEvents& events) override;
    [[nodiscard]] core::Status start_station(std::string_view ssid,
                                             std::string_view password) override;
    [[nodiscard]] core::Status start_access_point() override;
    void reconnect() override;
    void stop() override;

private:
    static void event_handler(void* context, const char* event_base,
                              std::int32_t event_id, void* event_data);
    [[nodiscard]] core::Status register_event_handlers();
    void unregister_event_handlers();
    void destroy_netifs();

    std::atomic<IWifiPlatformEvents*> events_{nullptr};
    core::AtomicComponentLifecycle lifecycle_{};
    void* wifi_event_handler_{nullptr};
    void* ip_event_handler_{nullptr};
    void* station_netif_{nullptr};
    void* access_point_netif_{nullptr};
    bool wifi_initialized_{false};
};

} // namespace espscreen::runtime
