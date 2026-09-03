#pragma once

#include "espscreen/core/application_board.hpp"
#include <array>
#include <cstddef>

namespace espscreen::apps {

struct BoardInfoModel final {
    static constexpr std::size_t max_headers = 8;

    std::array<char, 64> title{};
    std::array<char, 256> summary{};
    std::array<std::array<char, 180>, max_headers> headers{};
    std::size_t header_count{0};
};

[[nodiscard]] BoardInfoModel board_info_model(const core::ApplicationBoardInfo& board);

} // namespace espscreen::apps
