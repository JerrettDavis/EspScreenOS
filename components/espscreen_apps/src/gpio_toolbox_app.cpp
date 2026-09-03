#include "espscreen/apps/gpio_toolbox_app.hpp"
#include "espscreen/apps/gpio_toolbox_policy.hpp"
#include "espscreen/core/application_gpio.hpp"

namespace espscreen::apps {
namespace {
constexpr std::uint32_t owner = 0x4750;
}

core::Status GpioToolboxApp::start(core::ApplicationSurface surface) {
    view_.build(surface, *this);

    for (const auto& candidate : gpio_pin_catalog(board_.board_info())) {
        if (!gpio_.acquire_gpio(candidate.pin, owner)) continue;
        if (!leased_pins_.push_back(candidate.pin)) {
            (void)gpio_.release_gpio(candidate.pin, owner);
            break;
        }
        if (!gpio_.configure_output(candidate.pin)) {
            (void)gpio_.release_gpio(candidate.pin, owner);
            leased_pins_.pop_back();
            continue;
        }
        if (!view_.add_pin(candidate.header_name, candidate.pin)) {
            (void)gpio_.release_gpio(candidate.pin, owner);
            leased_pins_.pop_back();
            break;
        }
    }
    view_.finish();
    return core::Status::success();
}

void GpioToolboxApp::write(std::int16_t pin, bool high) {
    (void)gpio_.write_gpio(pin, high);
}

void GpioToolboxApp::stop() {
    view_.reset();
    for (const auto pin : leased_pins_) (void)gpio_.release_gpio(pin, owner);
    leased_pins_.clear();
}

} // namespace espscreen::apps
