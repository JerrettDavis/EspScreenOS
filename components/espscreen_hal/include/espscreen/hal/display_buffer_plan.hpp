#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace espscreen::hal {

enum class DisplayBufferPlacement : std::uint8_t { internal_dma, external };

struct DisplayBufferStrategy final {
    DisplayBufferPlacement draw_placement{};
    bool staging_required{};
};

struct DisplayBufferPlan final {
    std::size_t draw_buffer_bytes{};
    std::size_t staging_buffer_bytes{};
    std::size_t maximum_spi_transfer_bytes{};
    std::array<DisplayBufferStrategy, 2> strategies{{
        {DisplayBufferPlacement::internal_dma, false},
        {DisplayBufferPlacement::external, true},
    }};

    [[nodiscard]] constexpr explicit operator bool() const {
        return draw_buffer_bytes != 0 && staging_buffer_bytes != 0;
    }
};

[[nodiscard]] constexpr DisplayBufferPlan display_buffer_plan(
    std::uint16_t width, std::uint16_t draw_buffer_lines,
    std::uint16_t transfer_buffer_lines) {
    if (width == 0 || draw_buffer_lines == 0 || transfer_buffer_lines == 0) return {};
    const auto row_bytes = static_cast<std::size_t>(width) * 2U;
    const auto staging_bytes = row_bytes * transfer_buffer_lines;
    return {.draw_buffer_bytes = row_bytes * draw_buffer_lines,
            .staging_buffer_bytes = staging_bytes,
            .maximum_spi_transfer_bytes =
                (row_bytes * draw_buffer_lines > staging_bytes
                     ? row_bytes * draw_buffer_lines
                     : staging_bytes) +
                16U};
}

} // namespace espscreen::hal
