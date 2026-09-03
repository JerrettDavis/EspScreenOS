#include "espscreen/services/wifi_settings.hpp"

namespace espscreen::services {
namespace {
constexpr auto ssid_key = "wifi_ssid";
constexpr auto password_key = "wifi_pass";
}

core::Status WifiSettings::load_wifi(WifiCredentials& credentials) const {
    credentials = {};
    if (const auto status = settings_.get_text(
            ssid_key, credentials.ssid.data(), credentials.ssid.size()); !status)
        return status;
    return settings_.get_text(password_key, credentials.password.data(),
                              credentials.password.size());
}

core::Status WifiSettings::save_wifi(const WifiCredentials& credentials) {
    WifiCredentials previous{};
    const bool had_previous = load_wifi(previous).ok();
    if (const auto status = settings_.set_text(ssid_key, credentials.ssid.data()); !status)
        return status;
    if (const auto status = settings_.set_text(password_key, credentials.password.data());
        !status) {
        const auto rollback = had_previous
            ? settings_.set_text(ssid_key, previous.ssid.data())
            : settings_.erase(ssid_key);
        if (!rollback)
            return {core::ErrorCode::io_error, "Wi-Fi credential rollback failed"};
        return status;
    }
    return core::Status::success();
}

} // namespace espscreen::services
