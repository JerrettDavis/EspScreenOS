#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <span>

namespace espscreen::apps {

struct HardwarePreset final {
    const char* name;
    std::uint8_t intensity;
};

enum class HardwareTestCommand : std::uint8_t {
    test_motor,
    stop_motor,
    test_speaker,
    inspect_storage,
};

enum class HardwareEffectKind : std::uint8_t {
    none,
    stop_haptics,
    pulse_haptics,
    play_tone,
    inspect_storage,
};

enum class HardwareMessage : std::uint8_t {
    none,
    motor_off,
    enable_haptics,
    motor_preview,
    motor_started,
    motor_stopped,
    speaker_complete,
};

struct HardwareTestEffect final {
    HardwareEffectKind kind{HardwareEffectKind::none};
    HardwareMessage success_message{HardwareMessage::none};
    std::uint16_t duration_ms{0};
    std::uint16_t frequency_hz{0};
    std::uint8_t intensity{0};
    std::uint8_t volume{0};
    bool persist_enabled{false};
    bool persist_intensity{false};
};

class HardwareTestModel final {
public:
    static constexpr std::array presets{
        HardwarePreset{"High", 100}, HardwarePreset{"Med", 65},
        HardwarePreset{"Low", 35}, HardwarePreset{"Off", 0}};

    void restore(bool enabled, std::int32_t intensity);
    [[nodiscard]] HardwareTestEffect set_enabled(bool enabled);
    void set_intensity(std::int32_t intensity);
    [[nodiscard]] HardwareTestEffect commit_intensity() const;
    [[nodiscard]] HardwareTestEffect preview(std::uint8_t intensity);
    [[nodiscard]] HardwareTestEffect perform(HardwareTestCommand command) const;

    [[nodiscard]] bool enabled() const { return enabled_; }
    [[nodiscard]] std::uint8_t intensity() const { return intensity_; }

private:
    bool enabled_{true};
    std::uint8_t intensity_{60};
};

[[nodiscard]] bool format_hardware_message(HardwareMessage message, std::uint8_t intensity,
                                           std::span<char> destination);

} // namespace espscreen::apps
