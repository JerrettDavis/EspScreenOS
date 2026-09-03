#pragma once

#include "espscreen/core/application.hpp"
#include "espscreen/core/application_haptics.hpp"
#include "espscreen/core/application_host.hpp"
#include "espscreen/core/event_channel.hpp"
#include "espscreen/core/runtime_loop.hpp"
#include "espscreen/ui/shell.hpp"
#include "espscreen/ui/shell_capabilities.hpp"
#include "espscreen/ui/shell_view.hpp"

namespace espscreen::runtime {

class FirmwareShell final {
public:
    FirmwareShell(core::IApplicationHost& applications, core::IEventSource& events,
                  ui::ShellCapabilities capabilities,
                  core::IApplicationHaptics& haptics);

    [[nodiscard]] core::Status start();
    void stop();
    [[nodiscard]] core::IRuntimeWork& runtime_work() { return shell_; }

private:
    core::IApplicationHaptics& haptics_;
    ui::ShellView view_{};
    ui::Shell shell_;
};

} // namespace espscreen::runtime
