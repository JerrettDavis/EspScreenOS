#include "espscreen/hal/touch_geometry.hpp"
#include <algorithm>

namespace espscreen::hal {

std::int16_t map_touch_axis(std::int32_t raw,
                            std::int32_t calibration_start,
                            std::int32_t calibration_end,
                            std::uint16_t extent) {
    if (extent == 0 || calibration_start == calibration_end) return 0;
    const auto scaled = (raw - calibration_start) * extent /
                        (calibration_end - calibration_start);
    return static_cast<std::int16_t>(
        std::clamp<std::int32_t>(scaled, 0, extent - 1));
}

TouchPoint transform_touch_point(TouchPoint point,
                                 std::uint16_t width,
                                 std::uint16_t height,
                                 std::uint8_t rotation) {
    if (width == 0 || height == 0) return {};
    switch (rotation & 3U) {
    case 1: {
        const auto x = point.x;
        point.x = static_cast<std::int16_t>(width - 1 - point.y);
        point.y = x;
        break;
    }
    case 2:
        point.x = static_cast<std::int16_t>(width - 1 - point.x);
        point.y = static_cast<std::int16_t>(height - 1 - point.y);
        break;
    case 3: {
        const auto x = point.x;
        point.x = point.y;
        point.y = static_cast<std::int16_t>(height - 1 - x);
        break;
    }
    default:
        break;
    }
    point.x = std::clamp<std::int16_t>(point.x, 0,
                                       static_cast<std::int16_t>(width - 1));
    point.y = std::clamp<std::int16_t>(point.y, 0,
                                       static_cast<std::int16_t>(height - 1));
    return point;
}

} // namespace espscreen::hal
