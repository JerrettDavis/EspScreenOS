#pragma once
#include <cstdint>

namespace espscreen::core {

enum class CapabilityState : std::uint8_t {
    stopped,
    ready,
    degraded,
    failed,
};

struct PowerTelemetry final {
    std::int32_t battery_mv{-1};
    std::int8_t battery_percent{-1};
    bool usb_present{false};
    std::int32_t ambient_raw{-1};
};

struct StorageTelemetry final {
    bool mounted{false};
    std::uint64_t total_bytes{0};
    std::uint64_t free_bytes{0};
};

struct SystemTelemetry final {
    std::uint64_t uptime_ms{0};
    std::uint32_t free_heap{0};
    std::uint32_t minimum_free_heap{0};
    std::uint32_t largest_free_block{0};
    std::uint32_t free_psram{0};
    PowerTelemetry power{};
    StorageTelemetry storage{};
    CapabilityState display{CapabilityState::stopped};
    CapabilityState touch{CapabilityState::stopped};
    CapabilityState sd{CapabilityState::stopped};
    CapabilityState audio{CapabilityState::stopped};
    CapabilityState power_state{CapabilityState::stopped};
};

} // namespace espscreen::core
