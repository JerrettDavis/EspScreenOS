#pragma once
#include <cstdint>
#include <string_view>

namespace espscreen::core {

enum class HealthState : std::uint8_t { unknown, starting, healthy, degraded, failed, stopped };

struct HealthRecord final {
    std::string_view id{};
    HealthState state{HealthState::unknown};
    std::string_view detail{};
    std::uint64_t changed_at_us{0};
    bool critical{false};
};

} // namespace espscreen::core
