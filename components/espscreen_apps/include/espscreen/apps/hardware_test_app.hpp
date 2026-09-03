#pragma once

#include "espscreen/apps/hardware_test_model.hpp"
#include "espscreen/apps/hardware_test_view_port.hpp"
#include "espscreen/core/application.hpp"
#include "espscreen/core/application_tone.hpp"
#include "espscreen/core/application_haptics.hpp"
#include "espscreen/core/application_storage_status.hpp"
#include "espscreen/core/settings_store.hpp"

namespace espscreen::apps {

class HardwareTestApp final : public core::IApplication, private IHardwareTestActions {
public:
    HardwareTestApp(IHardwareTestView& view, core::IApplicationHaptics& haptics,
                    core::IApplicationTone& tone,
                    core::IApplicationStorageStatus& storage,
                    core::ISettingsStore& settings)
        : view_(view), haptics_(haptics), tone_(tone), storage_(storage),
          settings_(settings) {}

    [[nodiscard]] std::string_view id() const override { return "hardware"; }
    [[nodiscard]] std::string_view title() const override { return "Test"; }
    [[nodiscard]] core::Status start(core::ApplicationSurface surface) override;
    void stop() override;

private:
    void apply(HardwareTestEffect effect);
    void show_message(HardwareMessage message);
    void perform(HardwareViewAction action) override;

    IHardwareTestView& view_;
    core::IApplicationHaptics& haptics_;
    core::IApplicationTone& tone_;
    core::IApplicationStorageStatus& storage_;
    core::ISettingsStore& settings_;
    HardwareTestModel model_{};
};

} // namespace espscreen::apps
