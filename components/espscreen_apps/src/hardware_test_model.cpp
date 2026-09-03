#include "espscreen/apps/hardware_test_model.hpp"
#include <algorithm>
#include <cstdio>

namespace espscreen::apps {

void HardwareTestModel::restore(bool enabled, std::int32_t intensity) {
    enabled_ = enabled;
    set_intensity(intensity);
}

HardwareTestEffect HardwareTestModel::set_enabled(bool enabled) {
    enabled_ = enabled;
    return {.kind = enabled ? HardwareEffectKind::none : HardwareEffectKind::stop_haptics,
            .success_message = enabled ? HardwareMessage::none : HardwareMessage::motor_off,
            .persist_enabled = true};
}

void HardwareTestModel::set_intensity(std::int32_t intensity) {
    intensity_ = static_cast<std::uint8_t>(std::clamp<std::int32_t>(intensity, 0, 100));
}

HardwareTestEffect HardwareTestModel::commit_intensity() const {
    return {.kind = enabled_ ? HardwareEffectKind::pulse_haptics
                             : HardwareEffectKind::none,
            .duration_ms = 120,
            .intensity = intensity_,
            .persist_intensity = true};
}

HardwareTestEffect HardwareTestModel::preview(std::uint8_t intensity) {
    set_intensity(intensity);
    if (intensity_ == 0)
        return {.kind = HardwareEffectKind::stop_haptics,
                .success_message = HardwareMessage::motor_off};
    if (!enabled_)
        return {.success_message = HardwareMessage::enable_haptics};
    return {.kind = HardwareEffectKind::pulse_haptics,
            .success_message = HardwareMessage::motor_preview,
            .duration_ms = 500,
            .intensity = intensity_};
}

HardwareTestEffect HardwareTestModel::perform(HardwareTestCommand command) const {
    switch (command) {
    case HardwareTestCommand::test_motor:
        if (!enabled_) return {.success_message = HardwareMessage::enable_haptics};
        return {.kind = HardwareEffectKind::pulse_haptics,
                .success_message = HardwareMessage::motor_started,
                .duration_ms = 250,
                .intensity = intensity_};
    case HardwareTestCommand::stop_motor:
        return {.kind = HardwareEffectKind::stop_haptics,
                .success_message = HardwareMessage::motor_stopped};
    case HardwareTestCommand::test_speaker:
        return {.kind = HardwareEffectKind::play_tone,
                .success_message = HardwareMessage::speaker_complete,
                .duration_ms = 180,
                .frequency_hz = 880,
                .volume = 35};
    case HardwareTestCommand::inspect_storage:
        return {.kind = HardwareEffectKind::inspect_storage};
    }
    return {};
}

bool format_hardware_message(HardwareMessage message, std::uint8_t intensity,
                             std::span<char> destination) {
    if (destination.empty() || message == HardwareMessage::none) return false;
    const char* fixed = nullptr;
    switch (message) {
    case HardwareMessage::motor_off: fixed = "Motor is off"; break;
    case HardwareMessage::enable_haptics: fixed = "Enable haptics first"; break;
    case HardwareMessage::motor_started: fixed = "Motor test started"; break;
    case HardwareMessage::motor_stopped: fixed = "Motor stopped"; break;
    case HardwareMessage::speaker_complete: fixed = "Speaker test complete"; break;
    case HardwareMessage::motor_preview:
        return std::snprintf(destination.data(), destination.size(),
                             "Motor preview at %u%% | 500ms", intensity) > 0;
    case HardwareMessage::none: return false;
    }
    return std::snprintf(destination.data(), destination.size(), "%s", fixed) > 0;
}

} // namespace espscreen::apps
