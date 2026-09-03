#pragma once

#include "espscreen/core/application_haptics.hpp"
#include "espscreen/runtime/system_restart.hpp"

namespace espscreen::runtime {

class FirmwareSystemRestart final : public ISystemRestart {
public:
    explicit FirmwareSystemRestart(core::IApplicationHaptics& haptics)
        : haptics_(haptics) {}
    void restart() override;
private:
    core::IApplicationHaptics& haptics_;
};

} // namespace espscreen::runtime
