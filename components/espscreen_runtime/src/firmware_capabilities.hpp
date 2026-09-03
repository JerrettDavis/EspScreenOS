#pragma once

#include "espscreen/core/application_audio_input.hpp"
#include "espscreen/core/application_audio_output.hpp"
#include "espscreen/core/application_files.hpp"
#include "espscreen/core/application_haptics.hpp"
#include "espscreen/core/application_jobs.hpp"
#include "espscreen/core/application_storage_status.hpp"
#include "espscreen/core/application_tone.hpp"
#include "espscreen/core/settings_store.hpp"
#include "espscreen/hal/application_hardware_adapters.hpp"
#include "espscreen/runtime/firmware_application_files.hpp"
#include "espscreen/runtime/firmware_application_memory.hpp"
#include "espscreen/runtime/firmware_shell_display.hpp"
#include "espscreen/runtime/firmware_shell_feedback.hpp"
#include "espscreen/ui/shell_capabilities.hpp"

namespace espscreen::runtime {

struct FirmwareCapabilityDependencies final {
    const board::BoardDescriptor& board;
    resources::ResourceBroker& resources;
    hal::StorageDevice& storage;
    hal::HapticsController& haptics;
    hal::AudioDevice& audio;
    hal::DisplayDevice& display;
    ui::IShellInput& input;
};

class FirmwareCapabilities final {
public:
    explicit FirmwareCapabilities(FirmwareCapabilityDependencies dependencies);

    [[nodiscard]] ui::ShellCapabilities shell_capabilities(
        core::ISettingsStore& settings, core::IMonotonicClock& clock);
    [[nodiscard]] core::IApplicationHaptics& haptics() { return application_haptics_; }
    [[nodiscard]] core::IApplicationBoard& board() { return application_board_; }
    [[nodiscard]] core::IApplicationGpio& gpio() { return application_gpio_; }
    [[nodiscard]] core::IApplicationMemory& memory() { return application_memory_; }
    [[nodiscard]] core::IApplicationTone& tone() { return application_audio_; }
    [[nodiscard]] core::IApplicationAudioInput& audio_input() {
        return application_audio_;
    }
    [[nodiscard]] core::IApplicationAudioOutput& audio_output() {
        return application_audio_;
    }
    [[nodiscard]] core::IApplicationStorageStatus& storage_status() {
        return application_storage_;
    }
    [[nodiscard]] core::IApplicationTextFiles& text_files() {
        return application_files_;
    }
    [[nodiscard]] core::IApplicationDirectory& application_directory() {
        return application_files_;
    }

private:
    hal::ApplicationBoardAdapter application_board_;
    hal::ApplicationStorageAdapter application_storage_;
    hal::ApplicationHapticsAdapter application_haptics_;
    hal::ApplicationAudioAdapter application_audio_;
    hal::ApplicationGpioAdapter application_gpio_;
    FirmwareApplicationMemory application_memory_{};
    FirmwareApplicationFiles application_files_;
    FirmwareShellDisplay shell_display_;
    FirmwareShellFeedback shell_feedback_;
    ui::IShellInput& shell_input_;
};

} // namespace espscreen::runtime
