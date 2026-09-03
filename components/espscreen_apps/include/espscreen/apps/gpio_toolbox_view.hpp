#pragma once

#include "espscreen/apps/gpio_toolbox_view_port.hpp"
#include <array>
#include <cstddef>
#include <cstdint>

struct lv_event_t;
struct lv_obj_t;

namespace espscreen::apps {

class GpioToolboxView final : public IGpioToolboxView {
public:
    void build(core::ApplicationSurface root,
               IGpioToolboxActions& actions) override;
    [[nodiscard]] bool add_pin(const char* header_name, std::int16_t pin) override;
    void finish() override;
    void reset() override;

private:
    struct PinBinding final {
        IGpioToolboxActions* actions;
        std::int16_t pin;
    };
    static void pin_callback(lv_event_t* event);

    lv_obj_t* root_{nullptr};
    lv_obj_t* status_{nullptr};
    lv_obj_t* list_{nullptr};
    std::array<PinBinding, 32> bindings_{};
    std::size_t count_{0};
};

} // namespace espscreen::apps
