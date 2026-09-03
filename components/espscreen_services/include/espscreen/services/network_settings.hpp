#pragma once
#include "espscreen/core/status.hpp"
#include "espscreen/services/wifi_credentials.hpp"

namespace espscreen::services {

class INetworkSettings {
public:
    virtual ~INetworkSettings() = default;
    [[nodiscard]] virtual core::Status load_wifi(WifiCredentials& credentials) const = 0;
    [[nodiscard]] virtual core::Status save_wifi(const WifiCredentials& credentials) = 0;
};

} // namespace espscreen::services
