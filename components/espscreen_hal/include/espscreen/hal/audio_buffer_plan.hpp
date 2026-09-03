#pragma once
#include <cstddef>
#include <cstdint>
namespace espscreen::hal {
struct AudioBufferPlan final {
    std::size_t frames{0};
    [[nodiscard]] constexpr std::size_t stereo_samples() const { return frames * 2; }
    [[nodiscard]] constexpr std::size_t bytes() const { return stereo_samples() * sizeof(std::int16_t); }
};
inline constexpr AudioBufferPlan audio_buffer_plan{.frames = 1'024};
}
