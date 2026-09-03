#pragma once

#include "espscreen/apps/calculator_view_port.hpp"

struct lv_obj_t;
struct lv_event_t;

namespace espscreen::apps {

class CalculatorView final : public ICalculatorView {
public:
    void build(core::ApplicationSurface surface, std::string_view display,
               ICalculatorActions& actions) override;
    void render(std::string_view display) override;
    void reset() override;

private:
    static void button_callback(lv_event_t* event);

    lv_obj_t* root_{nullptr};
    lv_obj_t* display_{nullptr};
    ICalculatorActions* actions_{nullptr};
};

} // namespace espscreen::apps
