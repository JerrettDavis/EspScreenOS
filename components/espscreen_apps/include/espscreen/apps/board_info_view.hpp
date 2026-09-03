#pragma once

#include "espscreen/apps/board_info_view_port.hpp"

struct lv_obj_t;

namespace espscreen::apps {

class BoardInfoView final : public IBoardInfoView {
public:
    void build(core::ApplicationSurface surface, const BoardInfoModel& model) override;
    void reset() override;

private:
    lv_obj_t* root_{nullptr};
};

} // namespace espscreen::apps
