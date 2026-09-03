#pragma once

#include <cstdint>
#include <optional>

namespace espscreen::ui {

struct DisplayPowerSettings final {
    std::int32_t brightness{85};
    std::int32_t dim_percent{30};
    std::int32_t timeout_seconds{120};
};

enum class DisplayPowerState : std::uint8_t { awake, dimmed, off };

// Pure state machine for display-idle behavior. Hardware and clock concerns stay
// at the shell boundary, making the policy portable and host-testable.
class DisplayPowerPolicy final {
public:
    [[nodiscard]] std::uint8_t configure(DisplayPowerSettings settings);
    [[nodiscard]] std::optional<std::uint8_t> note_activity(std::uint32_t now_ms);
    [[nodiscard]] std::optional<std::uint8_t> update(std::uint32_t now_ms);

    [[nodiscard]] const DisplayPowerSettings& settings() const { return settings_; }
    [[nodiscard]] DisplayPowerState state() const { return state_; }

private:
    [[nodiscard]] std::uint8_t brightness_for(DisplayPowerState state) const;

    DisplayPowerSettings settings_{};
    DisplayPowerState state_{DisplayPowerState::awake};
    std::uint32_t last_activity_ms_{0};
};

} // namespace espscreen::ui
