#include "espscreen/apps/board_info_model.hpp"
#include <algorithm>
#include <cstdio>

namespace espscreen::apps {
namespace {

constexpr const char* text_or_empty(const char* text) { return text ? text : ""; }

} // namespace

BoardInfoModel board_info_model(const core::ApplicationBoardInfo& board) {
    BoardInfoModel model{};
    std::snprintf(model.title.data(), model.title.size(), "%s",
                  text_or_empty(board.display_name));
    std::snprintf(model.summary.data(), model.summary.size(),
                  "Profile  %s\nFamily  %s\nDisplay  %u x %u\n"
                  "Memory  %u MB flash | %u MB PSRAM\nStatus  %s",
                  text_or_empty(board.id), text_or_empty(board.family),
                  board.display_width, board.display_height, board.flash_mb,
                  board.psram_mb, text_or_empty(board.maturity));
    model.header_count = std::min<std::size_t>(board.header_count, model.headers.size());
    for (std::size_t index = 0; index < model.header_count; ++index) {
        std::snprintf(model.headers[index].data(), model.headers[index].size(), "%s\n%s",
                      text_or_empty(board.headers[index].name),
                      text_or_empty(board.headers[index].note));
    }
    return model;
}

} // namespace espscreen::apps
