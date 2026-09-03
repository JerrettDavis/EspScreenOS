#pragma once

#include "espscreen/core/system_telemetry.hpp"
#include "espscreen/hal/device_state.hpp"

namespace espscreen::hal {

[[nodiscard]] constexpr core::CapabilityState capability_state(const DeviceHealth& health) {
    switch (health.state) {
    case DeviceState::ready: return core::CapabilityState::ready;
    case DeviceState::degraded: return core::CapabilityState::degraded;
    case DeviceState::failed: return core::CapabilityState::failed;
    default: return core::CapabilityState::stopped;
    }
}

} // namespace espscreen::hal
