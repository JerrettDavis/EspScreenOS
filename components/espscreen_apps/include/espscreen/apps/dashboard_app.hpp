#pragma once

#include "espscreen/apps/dashboard_view_port.hpp"
#include "espscreen/core/application.hpp"

namespace espscreen::apps {

class DashboardApp final : public core::IApplication {
public:
    explicit DashboardApp(IDashboardView& view) : view_(view) {}
    [[nodiscard]] std::string_view id() const override { return "dashboard"; }
    [[nodiscard]] std::string_view title() const override { return "Status"; }
    [[nodiscard]] core::Status start(core::ApplicationSurface surface) override;
    void stop() override;
    void on_event(const core::Event& event) override;

private:
    IDashboardView& view_;
};

} // namespace espscreen::apps
