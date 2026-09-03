#pragma once

#include "espscreen/core/system_telemetry.hpp"
#include <array>

namespace espscreen::apps {

struct DashboardModel final {
    std::array<char, 40> uptime{};
    std::array<char, 64> memory{};
    std::array<char, 64> power{};
    std::array<char, 96> peripherals{};
};

[[nodiscard]] DashboardModel dashboard_model(const core::SystemTelemetry& telemetry);

} // namespace espscreen::apps
