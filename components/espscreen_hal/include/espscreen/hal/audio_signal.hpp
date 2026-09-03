#pragma once

#include <cstddef>
#include <cstdint>

namespace espscreen::hal {

[[nodiscard]] std::uint8_t effective_volume(std::uint8_t requested_percent,
                                            std::uint8_t master_percent);
[[nodiscard]] std::uint32_t pwm_duty(std::uint8_t volume_percent,
                                     std::uint32_t maximum_duty = 1023);
[[nodiscard]] std::size_t tone_frame_count(std::uint32_t sample_rate,
                                           std::uint16_t duration_ms);
void render_stereo_sine(std::int16_t* samples,
                        std::size_t frames,
                        std::uint32_t sample_rate,
                        std::uint16_t frequency_hz,
                        std::uint8_t volume_percent,
                        std::size_t frame_offset = 0);

} // namespace espscreen::hal
