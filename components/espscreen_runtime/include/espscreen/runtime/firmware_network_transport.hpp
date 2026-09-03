#pragma once

#include "espscreen/runtime/provisioning_server.hpp"
#include "espscreen/runtime/wifi_platform.hpp"
#include "espscreen/core/atomic_component_lifecycle.hpp"
#include <atomic>
#include <string_view>

namespace espscreen::runtime {

class FirmwareNetworkTransport final : public services::INetworkTransport,
                                       private IWifiPlatformEvents {
public:
    FirmwareNetworkTransport(IWifiPlatform& wifi, IProvisioningServer& provisioning_server)
        : wifi_(wifi), provisioning_server_(provisioning_server) {}

    [[nodiscard]] core::Status start(services::INetworkConnectionObserver& observer,
                                     services::IWifiProvisioner& provisioner) override;
    [[nodiscard]] core::Status start_station(std::string_view ssid,
                                             std::string_view password) override;
    [[nodiscard]] core::Status start_provisioning() override;
    void reconnect() override;
    void stop() override;

private:
    void wifi_connected(std::string_view ip_address) override;
    void wifi_disconnected() override;

    std::atomic<services::INetworkConnectionObserver*> observer_{nullptr};
    std::atomic<services::IWifiProvisioner*> provisioner_{nullptr};
    core::AtomicComponentLifecycle lifecycle_{};
    IWifiPlatform& wifi_;
    IProvisioningServer& provisioning_server_;
};

} // namespace espscreen::runtime
