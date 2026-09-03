#include "espscreen/apps/paint_app.hpp"
#include "espscreen/core/application_memory.hpp"

namespace espscreen::apps {

core::Status PaintApp::start(core::ApplicationSurface surface) {
    const auto requirements = view_.prepare(surface);
    canvas_buffer_ = memory_.allocate(requirements.bytes,
                                      core::ApplicationMemoryKind::external);
    if (!canvas_buffer_) {
        view_.reset();
        return {core::ErrorCode::unavailable, "paint canvas allocation failed"};
    }
    view_.attach_canvas(canvas_buffer_, requirements);
    return core::Status::success();
}

void PaintApp::stop() {
    view_.reset();
    if (canvas_buffer_) memory_.release(canvas_buffer_);
    canvas_buffer_ = nullptr;
}

} // namespace espscreen::apps
