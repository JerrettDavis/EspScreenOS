#pragma once

#include "espscreen/core/runtime_loop.hpp"

namespace espscreen::runtime {

class FirmwareClock final : public core::IRuntimeClock {
public:
    [[nodiscard]] std::int64_t now_us() const override;
    void delay_ms(std::uint32_t duration) override;
};

} // namespace espscreen::runtime
