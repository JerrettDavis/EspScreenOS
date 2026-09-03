#pragma once
#include "espscreen/core/status.hpp"
#include <cstdint>
namespace espscreen::core {

class IApplicationHaptics {
public:
    virtual ~IApplicationHaptics() = default;
    [[nodiscard]] virtual bool haptics_available() const = 0;
    [[nodiscard]] virtual Status pulse_haptics(std::uint8_t intensity,
                                               std::uint16_t duration_ms) = 0;
    virtual void stop_haptics(const char* reason) = 0;
};

} // namespace espscreen::core
