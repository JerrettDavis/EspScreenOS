#pragma once
#include "espscreen/core/component_lifecycle.hpp"
#include "espscreen/core/event_mailbox_port.hpp"
#include "espscreen/core/service.hpp"
#include "espscreen/services/network_connection_policy.hpp"
#include "espscreen/services/network_settings.hpp"
#include "espscreen/services/network_transport.hpp"
#include <cstdint>
#include <string_view>

namespace espscreen::services {

class NetworkService final : public core::IService,
                             private INetworkConnectionObserver,
                             private IWifiProvisioner {
public:
    NetworkService(INetworkSettings& settings, INetworkTransport& transport,
                   core::IEventOutbox& mailbox)
        : settings_(settings), transport_(transport), mailbox_(mailbox) {}

    [[nodiscard]] std::string_view id() const override { return "network"; }
    [[nodiscard]] core::Status start() override;
    void stop() override;
    [[nodiscard]] NetworkState state() const { return connection_.state(); }
    [[nodiscard]] const char* ip_address() const { return ip_; }

private:
    void network_connected(std::string_view ip_address) override;
    void network_disconnected() override;
    [[nodiscard]] core::Status provision_wifi(std::string_view form) override;
    void publish();

    INetworkSettings& settings_;
    INetworkTransport& transport_;
    core::IEventOutbox& mailbox_;
    NetworkConnectionPolicy connection_{};
    core::ComponentLifecycle lifecycle_{};
    char ip_[16]{"0.0.0.0"};
};

} // namespace espscreen::services
