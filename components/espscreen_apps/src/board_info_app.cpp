#include "espscreen/apps/board_info_app.hpp"
#include "espscreen/apps/board_info_model.hpp"
#include "espscreen/core/application_board.hpp"

namespace espscreen::apps {

core::Status BoardInfoApp::start(core::ApplicationSurface surface) {
    view_.build(surface, board_info_model(board_.board_info()));
    return core::Status::success();
}

void BoardInfoApp::stop() { view_.reset(); }

} // namespace espscreen::apps
