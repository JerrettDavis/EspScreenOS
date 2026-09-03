#pragma once
#include "espscreen/core/status.hpp"
#include <cstdint>

namespace espscreen::hal {

enum class DeviceState : std::uint8_t { stopped, starting, ready, degraded, failed };
struct DeviceHealth final { DeviceState state{DeviceState::stopped}; core::ErrorCode error{core::ErrorCode::ok}; const char* detail{""}; };
}
