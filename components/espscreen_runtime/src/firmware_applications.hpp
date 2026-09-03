#pragma once

#include "firmware_builtin_applications.hpp"
#include "espscreen/apps/lua_sd_app.hpp"
#include "espscreen/apps/sd_app_loader.hpp"
#include "espscreen/core/application_files.hpp"
#include "espscreen/core/application_host.hpp"
#include "espscreen/core/app_manager.hpp"

namespace espscreen::runtime {

class FirmwareApplications final : public apps::IApplicationDiscovery {
public:
    FirmwareApplications(BuiltinApplicationFeatures features,
                         BuiltinApplicationDependencies dependencies)
        : features_(features), builtins_(dependencies), manager_(dependencies.jobs) {}

    [[nodiscard]] core::Status start();
    void stop();
    [[nodiscard]] core::Status discover(core::IApplicationDirectory& directory) override;
    [[nodiscard]] core::IApplicationHost& host() { return manager_; }

private:
    BuiltinApplicationFeatures features_;
    FirmwareBuiltinApplications builtins_;
    core::AppManager manager_;
    apps::LuaSdAppFactory sd_app_factory_{};
    apps::SdAppLoader sd_apps_{sd_app_factory_};
};

} // namespace espscreen::runtime
