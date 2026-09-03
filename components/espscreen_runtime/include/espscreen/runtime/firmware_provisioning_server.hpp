#pragma once

#include "espscreen/runtime/provisioning_server.hpp"
#include "espscreen/runtime/system_restart.hpp"
#include "espscreen/core/atomic_component_lifecycle.hpp"
#include <atomic>
#include <cstdint>

namespace espscreen::runtime {

struct ProvisioningHttpCallbacks;

class FirmwareProvisioningServer final : public IProvisioningServer {
public:
    explicit FirmwareProvisioningServer(ISystemRestart& restart)
        : restart_(restart) {}
    [[nodiscard]] core::Status start(
        services::IWifiProvisioner& provisioner) override;
    void stop() override;

private:
    friend struct ProvisioningHttpCallbacks;
    int serve_setup(void* request);
    int serve_health(void* request);
    int save_wifi(void* request);

    std::atomic<services::IWifiProvisioner*> provisioner_{nullptr};
    std::atomic<void*> server_{nullptr};
    core::AtomicComponentLifecycle lifecycle_{};
    ISystemRestart& restart_;
};

} // namespace espscreen::runtime
