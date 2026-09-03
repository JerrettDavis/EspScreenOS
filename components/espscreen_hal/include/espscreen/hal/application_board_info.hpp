#pragma once
#include "espscreen/board/descriptor.hpp"
#include "espscreen/core/application_board.hpp"
namespace espscreen::hal {
[[nodiscard]] constexpr core::ApplicationBoardInfo application_board_info(const board::BoardDescriptor& descriptor) {
 core::ApplicationBoardInfo result{.id=descriptor.id,.display_name=descriptor.display_name,.family=descriptor.family,.maturity=descriptor.maturity,.display_width=descriptor.display.width,.display_height=descriptor.display.height,.flash_mb=descriptor.mcu.flash_mb,.psram_mb=descriptor.mcu.psram_mb,.header_count=descriptor.header_count};
 for(std::size_t i=0;i<descriptor.header_count&&i<result.headers.size();++i){const auto& source=descriptor.headers[i];result.headers[i]={.name=source.name,.note=source.note,.pins=source.pins,.pin_count=source.pin_count,.gpio=source.kind==board::HeaderKind::gpio};}
 return result;
}
}
