#include "espscreen/runtime/firmware_network_transport.hpp"

namespace espscreen::runtime {

core::Status FirmwareNetworkTransport::start(
    services::INetworkConnectionObserver& observer,
    services::IWifiProvisioner& provisioner) {
    const auto transition = lifecycle_.begin_start("network transport already active");
    if (!transition.status) return transition.status;
    if (!transition.acquire)
        return observer_.load() == &observer && provisioner_.load() == &provisioner
                   ? core::Status::success()
                   : core::Status{core::ErrorCode::conflict,
                                  "network transport already active"};

    observer_.store(&observer);
    provisioner_.store(&provisioner);
    if (const auto started = wifi_.start(*this); !started) {
        observer_.store(nullptr);
        provisioner_.store(nullptr);
        lifecycle_.stopped();
        return started;
    }
    lifecycle_.started();
    return core::Status::success();
}

core::Status FirmwareNetworkTransport::start_station(std::string_view ssid,
                                                     std::string_view password) {
    if (!lifecycle_.running())
        return {core::ErrorCode::conflict, "network transport is not ready"};
    return wifi_.start_station(ssid, password);
}

core::Status FirmwareNetworkTransport::start_provisioning() {
    if (!lifecycle_.running())
        return {core::ErrorCode::conflict, "network transport is not ready"};
    if (const auto access_point = wifi_.start_access_point(); !access_point)
        return access_point;
    auto* provisioner = provisioner_.load();
    if (!provisioner)
        return {core::ErrorCode::unavailable, "Wi-Fi provisioner unavailable"};
    return provisioning_server_.start(*provisioner);
}

void FirmwareNetworkTransport::reconnect() {
    if (lifecycle_.running()) wifi_.reconnect();
}

void FirmwareNetworkTransport::wifi_connected(std::string_view ip_address) {
    if (!lifecycle_.running()) return;
    if (auto* provisioner = provisioner_.load())
        (void)provisioning_server_.start(*provisioner);
    if (auto* observer = observer_.load()) observer->network_connected(ip_address);
}

void FirmwareNetworkTransport::wifi_disconnected() {
    if (!lifecycle_.running()) return;
    if (auto* observer = observer_.load()) observer->network_disconnected();
}

void FirmwareNetworkTransport::stop() {
    if (!lifecycle_.begin_stop()) return;
    observer_.store(nullptr);
    provisioner_.store(nullptr);
    provisioning_server_.stop();
    wifi_.stop();
    lifecycle_.stopped();
}

} // namespace espscreen::runtime
