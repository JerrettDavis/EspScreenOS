#include "espscreen/hal/touch_input_filter.hpp"

namespace espscreen::hal {
namespace {

std::uint32_t elapsed(std::uint32_t now, std::uint32_t then) {
    return now - then;
}

} // namespace

TouchPoint TouchInputFilter::update(bool sample_available,
                                    TouchPoint sample,
                                    std::uint32_t now_ms) {
    const bool sampled_pressed = sample_available && sample.pressed;
    if (sampled_pressed) {
        release_pending_ = false;
        if (!pressed_ &&
            (!has_released_ || elapsed(now_ms, last_release_ms_) >= repress_guard_ms))
            pressed_ = true;
        last_point_ = sample;
    } else if (pressed_) {
        if (!release_pending_) {
            release_pending_ = true;
            release_started_ms_ = now_ms;
        } else if (elapsed(now_ms, release_started_ms_) >= release_debounce_ms) {
            pressed_ = false;
            release_pending_ = false;
            has_released_ = true;
            last_release_ms_ = now_ms;
        }
    } else {
        release_pending_ = false;
    }

    return {.x = last_point_.x, .y = last_point_.y, .pressed = pressed_};
}

void TouchInputFilter::reset() {
    last_point_ = {};
    release_started_ms_ = 0;
    last_release_ms_ = 0;
    pressed_ = false;
    release_pending_ = false;
    has_released_ = false;
}

} // namespace espscreen::hal
