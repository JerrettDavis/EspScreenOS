#pragma once

#include "espscreen/core/status.hpp"
#include <string_view>

namespace espscreen::services {

class INetworkConnectionObserver {
public:
    virtual ~INetworkConnectionObserver() = default;
    virtual void network_connected(std::string_view ip_address) = 0;
    virtual void network_disconnected() = 0;
};

class IWifiProvisioner {
public:
    virtual ~IWifiProvisioner() = default;
    [[nodiscard]] virtual core::Status provision_wifi(std::string_view form) = 0;
};

// Consumer-owned port for platform-specific Wi-Fi and provisioning mechanics.
class INetworkTransport {
public:
    virtual ~INetworkTransport() = default;
    [[nodiscard]] virtual core::Status start(INetworkConnectionObserver& observer,
                                             IWifiProvisioner& provisioner) = 0;
    [[nodiscard]] virtual core::Status start_station(std::string_view ssid,
                                                     std::string_view password) = 0;
    [[nodiscard]] virtual core::Status start_provisioning() = 0;
    virtual void reconnect() = 0;
    virtual void stop() = 0;
};

} // namespace espscreen::services
