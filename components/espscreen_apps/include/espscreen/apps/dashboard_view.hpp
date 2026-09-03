#pragma once

#include "espscreen/apps/dashboard_view_port.hpp"

struct lv_obj_t;

namespace espscreen::apps {

class DashboardView final : public IDashboardView {
public:
    void build(core::ApplicationSurface surface) override;
    void render(const DashboardModel& model) override;
    void reset() override;
    [[nodiscard]] bool ready() const { return root_ != nullptr; }

private:
    lv_obj_t* root_{nullptr};
    lv_obj_t* uptime_{nullptr};
    lv_obj_t* memory_{nullptr};
    lv_obj_t* power_{nullptr};
    lv_obj_t* peripherals_{nullptr};
};

} // namespace espscreen::apps
