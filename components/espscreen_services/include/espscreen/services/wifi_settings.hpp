#pragma once

#include "espscreen/services/network_settings.hpp"
#include "espscreen/services/text_settings.hpp"

namespace espscreen::services {

class WifiSettings final : public INetworkSettings {
public:
    explicit WifiSettings(ITextSettings& settings) : settings_(settings) {}
    [[nodiscard]] core::Status load_wifi(WifiCredentials& credentials) const override;
    [[nodiscard]] core::Status save_wifi(const WifiCredentials& credentials) override;
private:
    ITextSettings& settings_;
};

} // namespace espscreen::services
