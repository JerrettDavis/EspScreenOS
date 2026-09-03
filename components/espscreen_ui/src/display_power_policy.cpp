#include "espscreen/ui/display_power_policy.hpp"

#include <algorithm>

namespace espscreen::ui {

std::uint8_t DisplayPowerPolicy::configure(DisplayPowerSettings settings) {
    settings.brightness = std::clamp<std::int32_t>(settings.brightness, 10, 100);
    settings.dim_percent = std::clamp<std::int32_t>(settings.dim_percent, 5, 80);
    settings.timeout_seconds = std::clamp<std::int32_t>(settings.timeout_seconds, 0, 600);
    settings_ = settings;
    return brightness_for(state_);
}

std::optional<std::uint8_t> DisplayPowerPolicy::note_activity(std::uint32_t now_ms) {
    last_activity_ms_ = now_ms;
    if (state_ == DisplayPowerState::awake)
        return std::nullopt;
    state_ = DisplayPowerState::awake;
    return brightness_for(state_);
}

std::optional<std::uint8_t> DisplayPowerPolicy::update(std::uint32_t now_ms) {
    if (settings_.timeout_seconds <= 0)
        return std::nullopt;

    // Unsigned subtraction intentionally preserves elapsed time across tick wrap.
    const auto idle_ms = now_ms - last_activity_ms_;
    const auto timeout_ms = static_cast<std::uint32_t>(settings_.timeout_seconds) * 1000U;
    const auto next = idle_ms >= timeout_ms
                          ? DisplayPowerState::off
                          : idle_ms >= timeout_ms / 2U ? DisplayPowerState::dimmed
                                                       : DisplayPowerState::awake;
    if (next == state_)
        return std::nullopt;
    state_ = next;
    return brightness_for(state_);
}

std::uint8_t DisplayPowerPolicy::brightness_for(DisplayPowerState state) const {
    if (state == DisplayPowerState::off)
        return 0;
    if (state == DisplayPowerState::dimmed)
        return static_cast<std::uint8_t>(std::min(settings_.brightness, settings_.dim_percent));
    return static_cast<std::uint8_t>(settings_.brightness);
}

} // namespace espscreen::ui
