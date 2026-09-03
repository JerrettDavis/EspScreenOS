#include "espscreen/apps/recorder_app.hpp"
#include "espscreen/core/application_audio_input.hpp"
#include "espscreen/core/application_audio_output.hpp"
#include "espscreen/core/application_jobs.hpp"
#include "espscreen/core/application_memory.hpp"
#include <algorithm>

namespace espscreen::apps {

core::Status RecorderApp::start(core::ApplicationSurface surface) {
    buffer_plan_ = recorder_buffer_plan(input_.audio_sample_rate());
    if (!buffer_plan_.valid())
        return {core::ErrorCode::invalid_argument, "unsupported recorder sample rate"};
    samples_ = static_cast<std::int16_t*>(memory_.allocate(
        buffer_plan_.bytes(), core::ApplicationMemoryKind::external_preferred));
    if (!samples_)
        return {core::ErrorCode::unavailable, "recorder buffer allocation failed"};
    playback_ = samples_ + buffer_plan_.capture_samples;
    sample_count_ = 0;
    captured_ = 0;
    state_ = RecorderState::ready;
    view_.build(surface, *this);
    return core::Status::success();
}

void RecorderApp::submit(RecorderCommand command) {
    if (!can_submit(command_.peek(), state_.load())) return;
    if (!command_.try_reserve(command)) return;
    state_ = pending_recorder_state(command);
    if (!jobs_.submit(*this)) {
        command_.reset();
        state_ = failed_recorder_state(command);
    }
}

void RecorderApp::run() {
    const auto command = command_.take();
    if (command == RecorderCommand::record) {
        std::size_t offset = 0;
        while (offset < buffer_plan_.capture_samples) {
            std::size_t received = 0;
            const auto result = input_.read_audio(
                samples_ + offset,
                std::min<std::size_t>(1024, buffer_plan_.capture_samples - offset), received, 100);
            if (!result) {
                sample_count_ = 0;
                captured_ = 0;
                state_ = RecorderState::record_error;
                return;
            }
            offset += received;
            if (received == 0) break;
        }
        sample_count_ = offset;
        captured_ = offset;
        state_ = RecorderState::captured;
    } else if (command == RecorderCommand::play) {
        if (sample_count_ == 0) {
            state_ = RecorderState::empty;
            return;
        }
        std::size_t offset = 0;
        while (offset < sample_count_) {
            const auto count = std::min<std::size_t>(
                buffer_plan_.playback_samples / 2, sample_count_ - offset);
            for (std::size_t index = 0; index < count; ++index)
                playback_[index * 2] = playback_[index * 2 + 1] = samples_[offset + index];
            if (!output_.write_audio(playback_, count)) {
                state_ = RecorderState::play_error;
                return;
            }
            offset += count;
        }
        state_ = RecorderState::ready;
    }
}

void RecorderApp::tick() {
    view_.render(present_recorder(state_.load()), captured_.load());
}

void RecorderApp::stop() {
    view_.reset();
    if (samples_) memory_.release(samples_);
    samples_ = playback_ = nullptr;
    sample_count_ = 0;
    captured_ = 0;
    command_.reset();
    state_ = RecorderState::ready;
    buffer_plan_ = {};
}

} // namespace espscreen::apps
