#include "firmware_applications.hpp"

namespace espscreen::runtime {

core::Status FirmwareApplications::start() {
    return builtins_.register_enabled(manager_, features_);
}

void FirmwareApplications::stop() {
    (void)manager_.clear();
    sd_apps_.reset();
}

core::Status FirmwareApplications::discover(core::IApplicationDirectory& directory) {
    return sd_apps_.discover(manager_, directory);
}

} // namespace espscreen::runtime
