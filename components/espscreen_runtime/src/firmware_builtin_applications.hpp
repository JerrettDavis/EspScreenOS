#pragma once

#include "espscreen/apps/board_info_app.hpp"
#include "espscreen/apps/board_info_view.hpp"
#include "espscreen/apps/calculator_app.hpp"
#include "espscreen/apps/calculator_view.hpp"
#include "espscreen/apps/dashboard_app.hpp"
#include "espscreen/apps/dashboard_view.hpp"
#include "espscreen/apps/gpio_toolbox_app.hpp"
#include "espscreen/apps/gpio_toolbox_view.hpp"
#include "espscreen/apps/hardware_test_app.hpp"
#include "espscreen/apps/hardware_test_view.hpp"
#include "espscreen/apps/notes_app.hpp"
#include "espscreen/apps/notes_view.hpp"
#include "espscreen/apps/paint_app.hpp"
#include "espscreen/apps/paint_view.hpp"
#include "espscreen/apps/recorder_app.hpp"
#include "espscreen/apps/recorder_view.hpp"
#include "espscreen/core/app_manager.hpp"
#include "espscreen/core/application_audio_input.hpp"
#include "espscreen/core/application_audio_output.hpp"
#include "espscreen/core/application_storage_status.hpp"
#include "espscreen/core/status.hpp"
#include "espscreen/core/settings_store.hpp"
#include "espscreen/core/application_tone.hpp"

namespace espscreen::runtime {

struct BuiltinApplicationFeatures final {
    bool dashboard;
    bool calculator;
    bool gpio_toolbox;
    bool recorder;
    bool paint;
    bool notes;
    bool hardware_test;
    bool board_info;
};

struct BuiltinApplicationDependencies final {
    core::IApplicationBoard& board;
    core::IApplicationGpio& gpio;
    core::IApplicationMemory& memory;
    core::IApplicationTextFiles& text_files;
    core::IApplicationJobs& jobs;
    core::IApplicationHaptics& haptics;
    core::IApplicationTone& tone;
    core::IApplicationAudioInput& audio_input;
    core::IApplicationAudioOutput& audio_output;
    core::IApplicationStorageStatus& storage_status;
    core::ISettingsStore& settings;
};

class FirmwareBuiltinApplications final {
public:
    explicit FirmwareBuiltinApplications(BuiltinApplicationDependencies dependencies);
    [[nodiscard]] core::Status register_enabled(
        core::AppManager& manager, const BuiltinApplicationFeatures& features);

private:
    apps::DashboardView dashboard_view_{};
    apps::DashboardApp dashboard_;
    apps::CalculatorView calculator_view_{};
    apps::CalculatorApp calculator_;
    apps::BoardInfoView board_info_view_{};
    apps::BoardInfoApp board_info_;
    apps::HardwareTestView hardware_test_view_{};
    apps::HardwareTestApp hardware_test_;
    apps::GpioToolboxView gpio_toolbox_view_{};
    apps::GpioToolboxApp gpio_toolbox_;
    apps::NotesView notes_view_{};
    apps::NotesApp notes_;
    apps::PaintView paint_view_{};
    apps::PaintApp paint_;
    apps::RecorderView recorder_view_{};
    apps::RecorderApp recorder_;
};

} // namespace espscreen::runtime
