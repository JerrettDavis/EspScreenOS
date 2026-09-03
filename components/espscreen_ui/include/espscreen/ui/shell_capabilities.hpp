#pragma once
#include "espscreen/core/clock.hpp"
#include "espscreen/core/settings_store.hpp"
#include "espscreen/ui/shell_input_policy.hpp"
#include <cstdint>

namespace espscreen::ui {

class IShellDisplay {
public:
    virtual ~IShellDisplay() = default;
    virtual void set_backlight(std::uint8_t percent) = 0;
};

class IShellFeedback {
public:
    virtual ~IShellFeedback() = default;
    virtual void set_audio_volume(std::uint8_t percent) = 0;
    virtual void pulse(std::uint8_t intensity, std::uint16_t duration_ms) = 0;
    virtual void tone(std::uint16_t hz, std::uint16_t duration_ms, std::uint8_t volume) = 0;
};

class IShellInput {
public:
    virtual ~IShellInput() = default;
    [[nodiscard]] virtual TouchSample touch_sample() const = 0;
};

struct ShellCapabilities final {
    core::ISettingsStore& settings;
    IShellDisplay& display;
    IShellFeedback& feedback;
    core::IMonotonicClock& clock;
    IShellInput& input;
};

} // namespace espscreen::ui
