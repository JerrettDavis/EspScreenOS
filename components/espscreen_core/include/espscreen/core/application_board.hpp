#pragma once
#include <array>
#include <cstdint>
namespace espscreen::core {

struct ApplicationHeader final {
    const char* name{""};
    const char* note{""};
    std::array<std::int8_t, 8> pins{};
    std::uint8_t pin_count{0};
    bool gpio{false};
};

struct ApplicationBoardInfo final {
    const char* id{""};
    const char* display_name{""};
    const char* family{""};
    const char* maturity{""};
    std::uint16_t display_width{0};
    std::uint16_t display_height{0};
    std::uint8_t flash_mb{0};
    std::uint8_t psram_mb{0};
    std::array<ApplicationHeader, 8> headers{};
    std::uint8_t header_count{0};
};

class IApplicationBoard {
public:
    virtual ~IApplicationBoard() = default;
    [[nodiscard]] virtual ApplicationBoardInfo board_info() const = 0;
};

} // namespace espscreen::core
