#include "espscreen/apps/hardware_test_app.hpp"
#include "espscreen/core/application_tone.hpp"
#include "espscreen/core/application_haptics.hpp"
#include "espscreen/core/application_storage_status.hpp"

namespace espscreen::apps {

core::Status HardwareTestApp::start(core::ApplicationSurface surface) {
    bool enabled = model_.enabled();
    std::int32_t intensity = model_.intensity();
    std::int32_t saved{};
    if (settings_.get_i32("haptic_en", saved).ok()) enabled = saved != 0;
    if (settings_.get_i32("haptic_pct", saved).ok()) intensity = saved;
    model_.restore(enabled, intensity);
    haptics_.stop_haptics("entering motor test screen");

    view_.build(surface,
                {.enabled = model_.enabled(),
                 .intensity = model_.intensity(),
                 .haptics_available = haptics_.haptics_available()},
                *this);
    return core::Status::success();
}

void HardwareTestApp::perform(HardwareViewAction action) {
    switch (action.kind) {
    case HardwareViewActionKind::enabled_changed:
        apply(model_.set_enabled(action.value != 0));
        break;
    case HardwareViewActionKind::intensity_changed:
        model_.set_intensity(action.value);
        view_.set_intensity(model_.intensity());
        break;
    case HardwareViewActionKind::intensity_committed:
        apply(model_.commit_intensity());
        break;
    case HardwareViewActionKind::preview:
        apply(model_.preview(static_cast<std::uint8_t>(action.value)));
        view_.set_intensity(model_.intensity());
        break;
    case HardwareViewActionKind::perform:
        apply(model_.perform(static_cast<HardwareTestCommand>(action.value)));
        break;
    }
}

void HardwareTestApp::apply(HardwareTestEffect effect) {
    if (effect.persist_enabled)
        (void)settings_.set_i32("haptic_en", model_.enabled() ? 1 : 0);
    if (effect.persist_intensity)
        (void)settings_.set_i32("haptic_pct", model_.intensity());

    bool succeeded = true;
    switch (effect.kind) {
    case HardwareEffectKind::none:
        break;
    case HardwareEffectKind::stop_haptics:
        haptics_.stop_haptics("hardware test action");
        break;
    case HardwareEffectKind::pulse_haptics:
        succeeded = haptics_.pulse_haptics(effect.intensity, effect.duration_ms).ok();
        if (!succeeded && effect.success_message != HardwareMessage::none)
            view_.set_message("Motor unavailable");
        break;
    case HardwareEffectKind::play_tone:
        succeeded = tone_.request_tone(effect.frequency_hz, effect.duration_ms,
                                       effect.volume).ok();
        if (!succeeded) view_.set_message("Speaker unavailable");
        break;
    case HardwareEffectKind::inspect_storage:
        view_.show_storage(storage_.storage_mounted(), storage_.storage_free_bytes());
        break;
    }
    if (succeeded) show_message(effect.success_message);
}

void HardwareTestApp::show_message(HardwareMessage message) {
    char text[64]{};
    if (format_hardware_message(message, model_.intensity(), text)) view_.set_message(text);
}

void HardwareTestApp::stop() {
    haptics_.stop_haptics("leaving motor test screen");
    view_.reset();
}

} // namespace espscreen::apps
