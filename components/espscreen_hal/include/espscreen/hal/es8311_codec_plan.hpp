#pragma once

#include <array>
#include <cstdint>

namespace espscreen::hal {

struct Es8311RegisterWrite final {
    std::uint8_t address;
    std::uint8_t value;
    std::uint16_t delay_after_ms{0};
};

// ES8311 slave-mode startup for 4.096 MHz MCLK, 16 kHz samples and
// 16-bit Philips I2S in a 32-bit stereo frame.
inline constexpr std::array es8311_startup_plan{
    Es8311RegisterWrite{0x00, 0x1F, 20},
    Es8311RegisterWrite{0x00, 0x00},
    Es8311RegisterWrite{0x00, 0x80},
    Es8311RegisterWrite{0x01, 0x3F},
    Es8311RegisterWrite{0x02, 0x00},
    Es8311RegisterWrite{0x03, 0x10},
    Es8311RegisterWrite{0x04, 0x20},
    Es8311RegisterWrite{0x05, 0x00},
    Es8311RegisterWrite{0x06, 0x04},
    Es8311RegisterWrite{0x07, 0x00},
    Es8311RegisterWrite{0x08, 0xFF},
    Es8311RegisterWrite{0x09, 0x0C},
    Es8311RegisterWrite{0x0A, 0x0C},
    Es8311RegisterWrite{0x0D, 0x01},
    Es8311RegisterWrite{0x0E, 0x02},
    Es8311RegisterWrite{0x12, 0x00},
    Es8311RegisterWrite{0x13, 0x10},
    Es8311RegisterWrite{0x14, 0x1A},
    Es8311RegisterWrite{0x17, 0xBF},
    Es8311RegisterWrite{0x1C, 0x6A},
    Es8311RegisterWrite{0x32, 0xBF},
    Es8311RegisterWrite{0x37, 0x08},
};

} // namespace espscreen::hal
