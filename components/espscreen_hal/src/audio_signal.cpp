#include "espscreen/hal/audio_signal.hpp"

#include <algorithm>
#include <cmath>

namespace espscreen::hal {
namespace {
constexpr double pi = 3.14159265358979323846;
}

std::uint8_t effective_volume(std::uint8_t requested_percent,
                              std::uint8_t master_percent) {
    const auto requested = std::min<std::uint32_t>(requested_percent, 100);
    const auto master = std::min<std::uint32_t>(master_percent, 100);
    return static_cast<std::uint8_t>(requested * master / 100);
}

std::uint32_t pwm_duty(std::uint8_t volume_percent, std::uint32_t maximum_duty) {
    const auto volume = std::min<std::uint32_t>(volume_percent, 100);
    return (maximum_duty * volume / 100) / 2;
}

std::size_t tone_frame_count(std::uint32_t sample_rate,
                             std::uint16_t duration_ms) {
    return static_cast<std::size_t>(sample_rate) * duration_ms / 1000;
}

void render_stereo_sine(std::int16_t* samples,
                        std::size_t frames,
                        std::uint32_t sample_rate,
                        std::uint16_t frequency_hz,
                        std::uint8_t volume_percent,
                        std::size_t frame_offset) {
    if (!samples) return;
    const auto volume = std::min<std::uint32_t>(volume_percent, 100);
    if (sample_rate == 0 || frequency_hz == 0 || volume == 0) {
        std::fill_n(samples, frames * 2, std::int16_t{0});
        return;
    }

    const double amplitude = 32767.0 * static_cast<double>(volume) / 100.0;
    const double radians_per_frame = 2.0 * pi * frequency_hz / sample_rate;
    for (std::size_t frame = 0; frame < frames; ++frame) {
        const auto value = static_cast<std::int16_t>(
            std::sin(radians_per_frame * static_cast<double>(frame_offset + frame)) *
            amplitude);
        samples[frame * 2] = value;
        samples[frame * 2 + 1] = value;
    }
}

} // namespace espscreen::hal
