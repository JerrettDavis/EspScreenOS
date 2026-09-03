#pragma once
#include "espscreen/generated/selected_board.hpp"

namespace espscreen::board {
inline constexpr const BoardDescriptor& selected_board() { return espscreen::generated::kSelectedBoard; }
}
