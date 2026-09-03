#include "espscreen/apps/calculator_app.hpp"

namespace espscreen::apps {

core::Status CalculatorApp::start(core::ApplicationSurface surface) {
    engine_.reset();
    view_.build(surface, engine_.display(), *this);
    return core::Status::success();
}

void CalculatorApp::stop() { view_.reset(); }

void CalculatorApp::press(std::string_view key) {
    engine_.press(key);
    view_.render(engine_.display());
}

} // namespace espscreen::apps
