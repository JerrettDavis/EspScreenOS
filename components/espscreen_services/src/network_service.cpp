#include "espscreen/services/network_service.hpp"
#include "espscreen/services/wifi_credentials.hpp"
#include <algorithm>
#include <cstring>

namespace espscreen::services {

core::Status NetworkService::start() {
    const auto transition = lifecycle_.begin_start();
    if (!transition.status || !transition.acquire) return transition.status;
    if (const auto started = transport_.start(*this, *this); !started) {
        lifecycle_.start_failed();
        return started;
    }

    WifiCredentials credentials{};
    if (settings_.load_wifi(credentials).ok()) {
        if (const auto station = transport_.start_station(
                credentials.ssid.data(), credentials.password.data()); station) {
            connection_.station_started();
            lifecycle_.started();
            publish();
            return station;
        }
    }

    const auto provisioning = transport_.start_provisioning();
    if (!provisioning) {
        transport_.stop();
        lifecycle_.start_failed();
        return provisioning;
    }
    connection_.provisioning_started();
    lifecycle_.started();
    publish();
    return core::Status::success();
}

void NetworkService::network_connected(std::string_view ip_address) {
    if (!lifecycle_.running()) return;
    const auto length = std::min(ip_address.size(), sizeof(ip_) - 1);
    std::copy_n(ip_address.data(), length, ip_);
    ip_[length] = '\0';
    (void)connection_.connected();
    publish();
}

void NetworkService::network_disconnected() {
    if (!lifecycle_.running()) return;
    const auto transition = connection_.disconnected();
    if (transition.intent == NetworkIntent::reconnect)
        transport_.reconnect();
    else if (transition.intent == NetworkIntent::publish)
        publish();
}

core::Status NetworkService::provision_wifi(std::string_view form) {
    if (!lifecycle_.running())
        return {core::ErrorCode::unavailable, "network service is not running"};
    WifiCredentials credentials;
    if (const auto parsed = parse_wifi_credentials(form, credentials); !parsed) return parsed;
    if (const auto saved = settings_.save_wifi(credentials); !saved) return saved;
    return core::Status::success();
}

void NetworkService::publish() {
    (void)mailbox_.post({.kind=core::EventKind::connectivity_changed,
                         .value0=static_cast<std::int32_t>(connection_.state())});
}

void NetworkService::stop() {
    if (!lifecycle_.begin_stop()) return;
    transport_.stop();
    connection_.stop();
    std::memcpy(ip_, "0.0.0.0", sizeof("0.0.0.0"));
    lifecycle_.stopped();
}

} // namespace espscreen::services
