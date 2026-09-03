#include "espscreen/core/runtime_loop.hpp"
#include <algorithm>

namespace espscreen::core {

void RuntimeLoop::step() {
    const auto loop_started = clock_.now_us();
    work_.tick();

    const auto handler_started = clock_.now_us();
    const auto requested_wait_ms = ui_.handle();
    const auto handler_us = clock_.now_us() - handler_started;
    if (first_frame_) {
        if (telemetry_ != nullptr) telemetry_->first_frame(handler_us);
        first_frame_ = false;
    }

    const bool animating = ui_.animations_running();
    const bool animation_started = !was_animating_ && animating;
    if (animation_started) {
        maximum_gap_us_ = maximum_handler_us_ = sum_gap_us_ = 0;
        animation_samples_ = late_samples_ = 0;
        last_loop_us_ = clock_.now_us();
    } else if (was_animating_) {
        const auto gap_us = loop_started - last_loop_us_;
        maximum_gap_us_ = std::max(maximum_gap_us_, gap_us);
        maximum_handler_us_ = std::max(maximum_handler_us_, handler_us);
        sum_gap_us_ += gap_us;
        ++animation_samples_;
        if (gap_us > 20000) ++late_samples_;
    }
    if (was_animating_ && !animating && animation_samples_ != 0 && telemetry_ != nullptr) {
        telemetry_->animation_pacing({
            .samples = animation_samples_,
            .late_samples = late_samples_,
            .average_gap_us = sum_gap_us_ / animation_samples_,
            .maximum_gap_us = maximum_gap_us_,
            .maximum_handler_us = maximum_handler_us_,
        });
    }
    was_animating_ = animating;
    if (!animation_started) last_loop_us_ = loop_started;
    clock_.delay_ms(std::clamp<std::uint32_t>(requested_wait_ms, 1, 16));
}

[[noreturn]] void RuntimeLoop::run() {
    while (true) step();
}

} // namespace espscreen::core
