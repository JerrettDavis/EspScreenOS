#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace espscreen::hal {

struct DisplayTransferArea final {
    std::int32_t x1{};
    std::int32_t y1{};
    std::int32_t x2{};
    std::int32_t y2{};

    [[nodiscard]] constexpr std::int32_t width() const { return x2 - x1 + 1; }
    [[nodiscard]] constexpr std::int32_t height() const { return y2 - y1 + 1; }
    [[nodiscard]] constexpr bool valid() const { return width() > 0 && height() > 0; }
};

struct DisplayTransferChunk final {
    std::int32_t row{};
    std::int32_t row_count{};
    std::int32_t y1{};
    std::int32_t y_end{};
    std::size_t source_offset{};
    std::size_t byte_count{};

    [[nodiscard]] constexpr explicit operator bool() const { return row_count > 0; }
};

[[nodiscard]] constexpr bool use_direct_display_transfer(bool draw_buffer_dma_capable,
                                                         bool source_dma_capable) {
    return draw_buffer_dma_capable && source_dma_capable;
}

[[nodiscard]] constexpr DisplayTransferChunk staged_display_transfer_chunk(
    DisplayTransferArea area, std::size_t staging_capacity, std::int32_t row) {
    if (!area.valid() || row < 0 || row >= area.height()) return {};
    const auto row_bytes = static_cast<std::size_t>(area.width()) * 2U;
    const auto capacity_rows = staging_capacity / row_bytes;
    if (capacity_rows == 0) return {};
    const auto rows = std::min<std::int32_t>(
        area.height() - row, static_cast<std::int32_t>(capacity_rows));
    return {.row = row,
            .row_count = rows,
            .y1 = area.y1 + row,
            .y_end = area.y1 + row + rows,
            .source_offset = static_cast<std::size_t>(row) * row_bytes,
            .byte_count = static_cast<std::size_t>(rows) * row_bytes};
}

} // namespace espscreen::hal
