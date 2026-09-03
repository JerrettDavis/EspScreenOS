#pragma once

#include "espscreen/services/network_transport.hpp"

namespace espscreen::runtime {

class IProvisioningServer {
public:
    virtual ~IProvisioningServer() = default;
    [[nodiscard]] virtual core::Status start(
        services::IWifiProvisioner& provisioner) = 0;
    virtual void stop() = 0;
};

} // namespace espscreen::runtime
