#pragma once
#include "espscreen/core/status.hpp"
#include <array>
#include <string_view>

namespace espscreen::services {

struct WifiCredentials final {
    std::array<char, 128> ssid{};
    std::array<char, 128> password{};
};

[[nodiscard]] core::Status parse_wifi_credentials(std::string_view form,
                                                  WifiCredentials& output);

} // namespace espscreen::services
