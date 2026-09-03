#pragma once

#include "espscreen/apps/paint_view_port.hpp"
#include "espscreen/core/application.hpp"
#include "espscreen/core/application_memory.hpp"

namespace espscreen::apps {

class PaintApp final : public core::IApplication {
public:
    PaintApp(IPaintView& view, core::IApplicationMemory& memory)
        : view_(view), memory_(memory) {}

    [[nodiscard]] std::string_view id() const override { return "paint"; }
    [[nodiscard]] std::string_view title() const override { return "Paint"; }
    [[nodiscard]] core::Status start(core::ApplicationSurface surface) override;
    void stop() override;

private:
    IPaintView& view_;
    core::IApplicationMemory& memory_;
    void* canvas_buffer_{nullptr};
};

} // namespace espscreen::apps
