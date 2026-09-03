#include "hello_app.hpp"

namespace espscreen::examples {

core::Status HelloApp::start(core::ApplicationSurface surface) noexcept {
  (void)surface;
  running_ = true;
  return core::Status::success();
}

void HelloApp::stop() noexcept { running_ = false; }

void HelloApp::tick() noexcept {
  if (!running_) {
    return;
  }
  // Publish work to the UI owner task or enqueue an application event here.
}

}  // namespace espscreen::examples
