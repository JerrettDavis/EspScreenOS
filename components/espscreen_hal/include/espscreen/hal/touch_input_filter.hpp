#pragma once

#include "espscreen/hal/touch_point.hpp"
#include <cstdint>

namespace espscreen::hal {

class TouchInputFilter final {
public:
    static constexpr std::uint32_t release_debounce_ms = 40;
    static constexpr std::uint32_t repress_guard_ms = 60;

    [[nodiscard]] TouchPoint update(bool sample_available,
                                    TouchPoint sample,
                                    std::uint32_t now_ms);
    void reset();

private:
    TouchPoint last_point_{};
    std::uint32_t release_started_ms_{0};
    std::uint32_t last_release_ms_{0};
    bool pressed_{false};
    bool release_pending_{false};
    bool has_released_{false};
};

} // namespace espscreen::hal
