#pragma once
#include "espscreen/core/status.hpp"
#include <cstdint>
namespace espscreen::core {

class IApplicationGpio {
public:
    virtual ~IApplicationGpio() = default;
    [[nodiscard]] virtual Status acquire_gpio(std::int16_t pin, std::uint32_t owner) = 0;
    [[nodiscard]] virtual Status release_gpio(std::int16_t pin, std::uint32_t owner) = 0;
    [[nodiscard]] virtual Status configure_output(std::int16_t pin) = 0;
    [[nodiscard]] virtual Status write_gpio(std::int16_t pin, bool high) = 0;
};

} // namespace espscreen::core
