#include "espscreen/apps/dashboard_app.hpp"
#include "espscreen/apps/dashboard_model.hpp"

namespace espscreen::apps {

core::Status DashboardApp::start(core::ApplicationSurface surface) {
    view_.build(surface);
    return core::Status::success();
}

void DashboardApp::stop() { view_.reset(); }

void DashboardApp::on_event(const core::Event& event) {
    if (event.kind != core::EventKind::telemetry_updated || !event.payload.telemetry)
        return;
    view_.render(dashboard_model(*event.payload.telemetry));
}

} // namespace espscreen::apps
