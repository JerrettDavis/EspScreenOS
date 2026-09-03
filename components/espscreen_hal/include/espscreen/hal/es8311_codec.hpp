#pragma once

#include "espscreen/board/descriptor.hpp"
#include "espscreen/core/status.hpp"
#include <cstdint>

namespace espscreen::hal {
class II2cBus;

class Es8311Codec final {
public:
    Es8311Codec(const board::AudioConfig& config, II2cBus& i2c)
        : config_(config), i2c_(i2c) {}
    ~Es8311Codec();
    [[nodiscard]] core::Status start();
    void stop();

private:
    [[nodiscard]] bool write(std::uint8_t address, std::uint8_t value);
    board::AudioConfig config_{};
    II2cBus& i2c_;
    bool acquired_{false};
};

} // namespace espscreen::hal
