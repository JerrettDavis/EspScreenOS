#pragma once

#include "espscreen/apps/board_info_view_port.hpp"
#include "espscreen/core/application.hpp"
#include "espscreen/core/application_board.hpp"

namespace espscreen::apps {

class BoardInfoApp final : public core::IApplication {
public:
    BoardInfoApp(IBoardInfoView& view, core::IApplicationBoard& board)
        : view_(view), board_(board) {}
    [[nodiscard]] std::string_view id() const override { return "board"; }
    [[nodiscard]] std::string_view title() const override { return "Board"; }
    [[nodiscard]] core::Status start(core::ApplicationSurface surface) override;
    void stop() override;

private:
    IBoardInfoView& view_;
    core::IApplicationBoard& board_;
};

} // namespace espscreen::apps
