#pragma once

#include "espscreen/core/status.hpp"

namespace espscreen::hal { class HapticsController; }

namespace espscreen::runtime {

class FirmwareHardwareSafety final {
public:
    explicit FirmwareHardwareSafety(hal::HapticsController& haptics)
        : haptics_(haptics) {}
    [[nodiscard]] core::Status start();

private:
    hal::HapticsController& haptics_;
};

} // namespace espscreen::runtime
