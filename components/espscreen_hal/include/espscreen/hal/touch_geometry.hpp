#pragma once

#include "espscreen/hal/touch_point.hpp"
#include <cstdint>

namespace espscreen::hal {

[[nodiscard]] std::int16_t map_touch_axis(std::int32_t raw,
                                          std::int32_t calibration_start,
                                          std::int32_t calibration_end,
                                          std::uint16_t extent);

[[nodiscard]] TouchPoint transform_touch_point(TouchPoint point,
                                               std::uint16_t width,
                                               std::uint16_t height,
                                               std::uint8_t rotation);

} // namespace espscreen::hal
