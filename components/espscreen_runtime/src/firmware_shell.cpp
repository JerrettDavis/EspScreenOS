#include "firmware_shell.hpp"

namespace espscreen::runtime {

FirmwareShell::FirmwareShell(core::IApplicationHost& applications,
                             core::IEventSource& events,
                             ui::ShellCapabilities capabilities,
                             core::IApplicationHaptics& haptics)
    : haptics_(haptics),
      shell_(applications, events, capabilities, view_) {}

core::Status FirmwareShell::start() {
    const auto result = shell_.start();
    if (!result) {
        haptics_.stop_haptics("UI initialization failure");
        shell_.stop();
    }
    return result;
}

void FirmwareShell::stop() { shell_.stop(); }

} // namespace espscreen::runtime
