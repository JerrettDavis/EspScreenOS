#include "firmware_capabilities.hpp"

namespace espscreen::runtime {

FirmwareCapabilities::FirmwareCapabilities(FirmwareCapabilityDependencies dependencies)
    : application_board_(dependencies.board),
      application_storage_(dependencies.board.storage, dependencies.storage),
      application_haptics_(dependencies.haptics),
      application_audio_(dependencies.board.audio, dependencies.audio),
      application_gpio_(dependencies.resources),
      application_files_(application_storage_),
      shell_display_(dependencies.display),
      shell_feedback_(dependencies.audio, dependencies.haptics),
      shell_input_(dependencies.input) {}

ui::ShellCapabilities FirmwareCapabilities::shell_capabilities(
    core::ISettingsStore& settings, core::IMonotonicClock& clock) {
    return {.settings = settings,
            .display = shell_display_,
            .feedback = shell_feedback_,
            .clock = clock,
            .input = shell_input_};
}

} // namespace espscreen::runtime
