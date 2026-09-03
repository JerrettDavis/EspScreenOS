#pragma once

#include "espscreen/core/status.hpp"
#include <cstdint>

namespace espscreen::core {

class IApplicationTone {
public:
    virtual ~IApplicationTone() = default;
    [[nodiscard]] virtual Status request_tone(std::uint16_t hz,
                                              std::uint16_t duration_ms,
                                              std::uint8_t volume) = 0;
};

} // namespace espscreen::core
