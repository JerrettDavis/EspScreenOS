#pragma once
#include "espscreen/core/system_telemetry.hpp"
#include <cstdint>

namespace espscreen::core {

enum class EventKind : std::uint16_t {
    boot_started,
    boot_complete,
    health_changed,
    telemetry_updated,
    touch_pressed,
    touch_released,
    storage_changed,
    connectivity_changed,
    app_changed,
    setting_changed,
    shutdown_requested,
    user = 0x8000,
};

struct EventPayload final {
    const SystemTelemetry* telemetry{nullptr};
    [[nodiscard]] constexpr bool borrowed() const { return telemetry != nullptr; }
};

struct Event final {
    EventKind kind{EventKind::boot_started};
    std::uint64_t timestamp_us{0};
    std::uint32_t source{0};
    std::int32_t value0{0};
    std::int32_t value1{0};
    EventPayload payload{};
    [[nodiscard]] constexpr bool queue_safe() const { return !payload.borrowed(); }
};

} // namespace espscreen::core
