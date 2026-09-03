#include "firmware_services.hpp"
#include <array>

namespace espscreen::runtime {

FirmwareServices::FirmwareServices(FirmwareServiceDependencies dependencies)
    : diagnostics_source_(dependencies.display, dependencies.touch,
                          dependencies.storage, dependencies.audio,
                          dependencies.power),
      system_restart_(dependencies.haptics), provisioning_server_(system_restart_),
      network_transport_(wifi_platform_, provisioning_server_),
      settings_(settings_backend_), wifi_settings_(settings_), assets_(asset_store_),
      diagnostics_(diagnostics_source_, dependencies.events, dependencies.clock),
      network_(wifi_settings_, network_transport_, dependencies.mailbox),
      manager_(dependencies.clock), runtime_work_(manager_) {}

core::Status FirmwareServices::start() {
    const std::array catalog{
        core::ServiceRegistration{settings_, false},
        core::ServiceRegistration{assets_, false},
        core::ServiceRegistration{diagnostics_, false},
        core::ServiceRegistration{network_, false},
    };
    auto result = manager_.register_services(catalog);
    if (result) result = manager_.start_all();
    if (!result) manager_.clear();
    return result;
}

void FirmwareServices::stop() { manager_.clear(); }

} // namespace espscreen::runtime
