#pragma once

#include <cstdint>

namespace espscreen::apps {

enum class RecorderCommand : std::uint8_t { none, record, play };
enum class RecorderState : std::uint8_t {
    ready,
    recording,
    playing,
    captured,
    record_error,
    play_error,
    empty,
};

struct RecorderPresentation final {
    const char* status;
    bool include_sample_count;
};

[[nodiscard]] constexpr bool can_submit(RecorderCommand pending, RecorderState state) {
    return pending == RecorderCommand::none && state != RecorderState::recording &&
           state != RecorderState::playing;
}

[[nodiscard]] constexpr RecorderState pending_recorder_state(RecorderCommand command) {
    return command == RecorderCommand::record ? RecorderState::recording
                                               : RecorderState::playing;
}

[[nodiscard]] constexpr RecorderState failed_recorder_state(RecorderCommand command) {
    return command == RecorderCommand::record ? RecorderState::record_error
                                               : RecorderState::play_error;
}

[[nodiscard]] constexpr RecorderPresentation present_recorder(RecorderState state) {
    switch (state) {
    case RecorderState::recording: return {"Recording...", false};
    case RecorderState::playing: return {"Playing...", false};
    case RecorderState::captured: return {"Captured %u samples", true};
    case RecorderState::record_error: return {"Microphone unavailable", false};
    case RecorderState::play_error: return {"Speaker unavailable", false};
    case RecorderState::empty: return {"Record something first", false};
    default: return {"Ready | two-second clips", false};
    }
}

} // namespace espscreen::apps
