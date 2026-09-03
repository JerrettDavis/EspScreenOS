#pragma once

#include <cstddef>
#include <cstdint>

namespace espscreen::apps {

struct RecorderBufferPlan final {
    std::size_t capture_samples{0};
    std::size_t playback_samples{0};

    [[nodiscard]] constexpr bool valid() const { return capture_samples != 0; }
    [[nodiscard]] constexpr std::size_t total_samples() const {
        return capture_samples + playback_samples;
    }
    [[nodiscard]] constexpr std::size_t bytes() const {
        return total_samples() * sizeof(std::int16_t);
    }
};

[[nodiscard]] constexpr RecorderBufferPlan recorder_buffer_plan(std::uint32_t sample_rate) {
    constexpr std::uint32_t maximum_sample_rate = 48'000;
    constexpr std::size_t clip_seconds = 2;
    constexpr std::size_t playback_frames = 1'024;
    if (sample_rate == 0 || sample_rate > maximum_sample_rate) return {};
    return {
        .capture_samples = static_cast<std::size_t>(sample_rate) * clip_seconds,
        .playback_samples = playback_frames * 2,
    };
}

} // namespace espscreen::apps
