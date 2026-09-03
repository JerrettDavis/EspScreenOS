#pragma once

#include "espscreen/apps/board_info_model.hpp"
#include "espscreen/core/application.hpp"

namespace espscreen::apps {

class IBoardInfoView {
public:
    virtual ~IBoardInfoView() = default;
    virtual void build(core::ApplicationSurface surface,
                       const BoardInfoModel& model) = 0;
    virtual void reset() = 0;
};

} // namespace espscreen::apps
