#pragma once
#include <algorithm>
#include <cstdint>
namespace espscreen::hal {

struct HapticsPulsePlan final {
    bool stop{true};
    std::uint8_t intensity_percent{0};
    std::uint8_t running_duty_percent{0};
    std::uint32_t duration_ms{0};
    std::uint8_t startup_kick_duty_percent{0};
    std::uint32_t startup_kick_ms{0};
};

class HapticsLogic final {
public:
    explicit HapticsLogic(std::uint8_t minimum_duty = 20)
        : minimum_duty_(std::clamp<std::uint8_t>(minimum_duty, 1, 100)) {}

    [[nodiscard]] std::uint8_t map_intensity(int percent) const {
        const auto normalized = std::clamp(percent, 0, 100);
        if (normalized == 0) return 0;
        return static_cast<std::uint8_t>(
            minimum_duty_ + ((normalized - 1) * (100 - minimum_duty_)) / 99);
    }

    [[nodiscard]] HapticsPulsePlan plan_pulse(
        int intensity_percent,
        std::uint32_t duration_ms,
        bool startup_kick_enabled,
        std::uint8_t startup_kick_duty_percent,
        std::uint32_t startup_kick_ms) const {
        const auto intensity = std::clamp(intensity_percent, 0, 100);
        const auto duration = std::min<std::uint32_t>(duration_ms, 5000);
        if (intensity == 0 || duration == 0) return {};

        const auto running_duty = map_intensity(intensity);
        const auto kick_duty = std::min<std::uint8_t>(startup_kick_duty_percent, 100);
        const bool use_kick = startup_kick_enabled && startup_kick_ms > 0 &&
                              running_duty < kick_duty;
        return {
            .stop = false,
            .intensity_percent = static_cast<std::uint8_t>(intensity),
            .running_duty_percent = running_duty,
            .duration_ms = duration,
            .startup_kick_duty_percent = use_kick ? kick_duty : std::uint8_t{0},
            .startup_kick_ms = use_kick ? startup_kick_ms : std::uint32_t{0},
        };
    }

    [[nodiscard]] static std::uint32_t pwm_duty(std::uint8_t duty_percent,
                                                 std::uint32_t maximum_duty = 1023) {
        return static_cast<std::uint32_t>(std::min<std::uint8_t>(duty_percent, 100)) *
               maximum_duty / 100;
    }

private:
    std::uint8_t minimum_duty_;
};
}
