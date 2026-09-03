#include "espscreen/apps/dashboard_model.hpp"
#include <cstdio>

namespace espscreen::apps {
namespace {

constexpr const char* availability(core::CapabilityState state) {
    return state == core::CapabilityState::ready ? "ready" : "limited";
}

} // namespace

DashboardModel dashboard_model(const core::SystemTelemetry& telemetry) {
    DashboardModel model{};
    std::snprintf(model.uptime.data(), model.uptime.size(), "Uptime %llus",
                  static_cast<unsigned long long>(telemetry.uptime_ms / 1000));
    std::snprintf(model.memory.data(), model.memory.size(),
                  "%lu KB free | %lu KB PSRAM",
                  static_cast<unsigned long>(telemetry.free_heap / 1024),
                  static_cast<unsigned long>(telemetry.free_psram / 1024));
    if (telemetry.power.battery_mv >= 0) {
        std::snprintf(model.power.data(), model.power.size(), "Battery %ld mV | %d%%",
                      static_cast<long>(telemetry.power.battery_mv),
                      telemetry.power.battery_percent);
    } else {
        std::snprintf(model.power.data(), model.power.size(),
                      "Battery sensing unavailable");
    }
    std::snprintf(model.peripherals.data(), model.peripherals.size(),
                  "Touch %s | SD %s | Audio %s", availability(telemetry.touch),
                  telemetry.storage.mounted ? "ready" : "absent",
                  availability(telemetry.audio));
    return model;
}

} // namespace espscreen::apps
