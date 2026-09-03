#pragma once

#include "espscreen/apps/dashboard_model.hpp"
#include "espscreen/core/application.hpp"

namespace espscreen::apps {

class IDashboardView {
public:
    virtual ~IDashboardView() = default;
    virtual void build(core::ApplicationSurface surface) = 0;
    virtual void render(const DashboardModel& model) = 0;
    virtual void reset() = 0;
};

} // namespace espscreen::apps
