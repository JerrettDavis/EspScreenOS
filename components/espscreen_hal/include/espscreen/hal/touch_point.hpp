#pragma once

#include <cstdint>

namespace espscreen::hal {

struct TouchPoint final {
    std::int16_t x{0};
    std::int16_t y{0};
    bool pressed{false};
};

} // namespace espscreen::hal
