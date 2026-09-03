#pragma once

#include "espscreen/core/status.hpp"
#include <string_view>

namespace espscreen::runtime {

class IWifiPlatformEvents {
public:
    virtual ~IWifiPlatformEvents() = default;
    virtual void wifi_connected(std::string_view ip_address) = 0;
    virtual void wifi_disconnected() = 0;
};

// Consumer-owned port for the ESP-independent network transport coordinator.
class IWifiPlatform {
public:
    virtual ~IWifiPlatform() = default;
    [[nodiscard]] virtual core::Status start(IWifiPlatformEvents& events) = 0;
    [[nodiscard]] virtual core::Status start_station(std::string_view ssid,
                                                     std::string_view password) = 0;
    [[nodiscard]] virtual core::Status start_access_point() = 0;
    virtual void reconnect() = 0;
    virtual void stop() = 0;
};

} // namespace espscreen::runtime
